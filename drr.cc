#include "drr.h"

namespace ns3 {

DRR::DRR() : active_queue(0) {}

/**
 * Dequeues the next scheduled packet and then updates
 * the active_queue index and next_deficit_counter vector
 * \returns packet that has been dequeued
 */
Ptr<Packet> DRR::Dequeue()
{
    std::vector<TrafficClass*> queues = GetQueues();
    // if (next_active_queue >= queues.size() || queues[next_active_queue]->IsEmpty())
    // {
    //     std::cout << "Active queue is invalid or empty during Dequeue." << std::endl;
    //     return nullptr;
    // }

    Ptr<Packet> pkt = queues[next_active_queue]->Dequeue();

    if (pkt != nullptr)
    {
        // std::cout << "Got dequeued: " << pkt->GetSize() << std::endl;
        // std::cout << "Dequeued from queue index: " << next_active_queue << std::endl;

        active_queue = next_active_queue;
        deficit_counter = next_deficit_counter;
    }

    return pkt;
}

Ptr<Packet> DRR::Remove()
{
    std::vector<TrafficClass*> queues = GetQueues();
    if (next_active_queue >= queues.size() || queues[next_active_queue]->IsEmpty())
    {
        std::cout << "Active queue is invalid or empty during Remove." << std::endl;
        return nullptr;
    }

    Ptr<Packet> pkt = queues[next_active_queue]->Dequeue(); 

    if (pkt != nullptr)
    {
        std::cout << "Got removed: " << pkt->GetSize() << std::endl;
        std::cout << "Removed from queue index: " << next_active_queue << std::endl;

        active_queue = next_active_queue;
        deficit_counter = next_deficit_counter;
    }

    return pkt;
}

/**
 * Finds the next scheduled packet by looping through the TrafficClasses
 * in q_class. The next scheduled packet will be from the first queue that
 * has enough deficit to send out a packet. Each new loop of the
 * queues results in an increase in deficit. The deficit amount is
 * decided based on the TrafficClass's weight variable.
 * 
 * Once this queue is found, Schedule will simply peek and return
 * the front packet.
 * 
 * \returns next scheduled packet
 */
Ptr<const Packet> DRR::Schedule() const
{
    std::vector<TrafficClass*> queues = GetQueues();
    uint32_t numQueues = queues.size();

    if (numQueues == 0)
    {
        std::cout << "No queues to serve." << std::endl;
        return nullptr;
    }

    // Initialize working state from current scheduler state
    next_active_queue = active_queue;
    next_deficit_counter = deficit_counter;

    // Check if all queues are empty
    bool allEmpty = true;
    for (auto q : queues)
    {
        if (!q->IsEmpty())
        {
            allEmpty = false;
            break;
        }
    }

    if (allEmpty)
    {
        std::cout << "All queues are empty." << std::endl;
        return nullptr;
    }

    // Loop to find a queue that can serve a packet
    for (uint32_t attempts = 0; attempts < numQueues; ++attempts)
    {
        TrafficClass* queue = queues[next_active_queue];

        if (!queue->IsEmpty())
        {
            next_deficit_counter[next_active_queue] += queue->GetWeight();
            uint32_t pktSize = queue->Peek()->GetSize();

            if (pktSize <= next_deficit_counter[next_active_queue])
            {
                next_deficit_counter[next_active_queue] -= pktSize;
                return queue->Peek();
            }
        }

        next_active_queue = (next_active_queue + 1) % numQueues;
    }

    return nullptr;
}

/**
 * Adds a TrafficClass to q_class vector
 * \param trafficClass TrafficClass to add
 */
void DRR::AddQueue(TrafficClass* trafficClass)
{
    // Add the new queue to the base DiffServ queue list
    DiffServ::AddQueue(trafficClass);

    // Initialize its deficit counter to 0
    deficit_counter.push_back(0);
    next_deficit_counter.push_back(0);
}

} // namespace ns3

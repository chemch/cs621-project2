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

    Ptr<Packet> dequeued_packet = DiffServ::Dequeue();


    if (dequeued_packet)
    {
        active_queue = next_active_queue;
        deficit_counter = next_deficit_counter;
        return dequeued_packet;
    }
    return nullptr;
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
   // get traffic class queues
    std::vector<TrafficClass*> queues = GetQueues();

    // check that there are queues to serve
    if (queues.size() == 0)
    {
        std::cout << "No queues to serve." << std::endl;
        return nullptr;
    }

    // reset to current deficit
    next_active_queue = active_queue;
    next_deficit_counter = deficit_counter;

    // check first that there are any packets
    uint32_t empty_count = 0;
    for (int i = 0; i < queues.size(); i++)
    {
        if (queues[i]->IsEmpty())
        {
            empty_count++;
        }
    }
    if (empty_count == queues.size())
    {
        std::cout << "All queues are empty." << std::endl;
        return nullptr;
    }

    // drr algorithm 
    while (true)
    {
        if (!queues[next_active_queue]->IsEmpty())
        {
            

            next_deficit_counter[next_active_queue] = queues[next_active_queue]->GetWeight() + next_deficit_counter[next_active_queue];
            // std::cout << "Deficit counter for: " << next_active_queue << ": " << next_deficit_counter[next_active_queue] << std::endl;
            uint32_t packet_size = queues[next_active_queue]->Peek()->GetSize();
            if (packet_size <= next_deficit_counter[next_active_queue])
            {
                // std::cout << "Packet size to remove: " << packet_size << std::endl;
                next_deficit_counter[next_active_queue] = next_deficit_counter[next_active_queue] - packet_size;
                return queues[next_active_queue]->Peek();
            }
        }
        next_active_queue = (next_active_queue + 1) % queues.size();
    }
    return queues[next_active_queue]->Peek();
}

/**
 * Adds a TrafficClass to q_class vector
 * \param trafficClass TrafficClass to add
 */
void DRR::AddQueue(TrafficClass* trafficClass)
{
    DiffServ::AddQueue(trafficClass);
    deficit_counter.push_back(0);
}

} // namespace ns3

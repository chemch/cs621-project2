#include "drr.h"
#include "ns3/log.h"
#include "diff-serv.h"
#include "ns3/packet.h"
#include "ns3/queue.h"

namespace ns3 {
    DRR::DRR() : currentQueue(0) {}

    /**
     * \brief Retrieves the list of traffic classes.
     * \details This function returns the list of traffic classes associated with the DiffServ instance.
     */
    std::vector<TrafficClass*> DRR::GetQueues() const
    {
        return q_class;
    }

    /**
     * Dequeues the next scheduled packet and then updates
     * the active_queue index and next_deficit_counter vector
     * \returns packet that has been dequeued
     */
    Ptr<Packet> DRR::Dequeue()
    {
        // Use Schedule to find the next packet to dequeue
        Ptr<Packet> dequeuePkt = DiffServ::Dequeue();

        // Check if the dequeue operation was successful
        if (dequeuePkt)
        {
            // Update the next active queue and quantum
            // Basically, we are moving to the next queue
            // and setting the quantum for the next round
            currentQueue = nextQueue;
            queueQuantums = tempQueueQuantums;

            return dequeuePkt;
        }
        // If dequeue failed, log an error message
        else 
        {
            NS_LOG_UNCOND("Dequeue failed: No packet to dequeue.");
        }

        return nullptr;
    }

    /**
     * Removes the next scheduled packet from the active queue
     * and updates the active_queue index and next_deficit_counter vector
     * \returns packet that has been removed
     */
    Ptr<Packet> DRR::Remove()
    {
        // Get queues to check if they are empty
        std::vector<TrafficClass*> queues = GetQueues();

        // Check if there are any queues to serve
        if (nextQueue >= queues.size() || queues[nextQueue]->IsEmpty())
        {
            NS_LOG_UNCOND("No queues to serve or the next active queue is empty.");
            return nullptr;
        }

        // Dequeue the packet from the next active queue
        Ptr<Packet> removePkt = queues[nextQueue]->Dequeue(); 

        if (removePkt != nullptr)
        {
            NS_LOG_UNCOND("Packet removed from queue: " << nextQueue);

            // Roll over to the next queue and update the quantum
            currentQueue = nextQueue;
            queueQuantums = tempQueueQuantums;
        }

        return removePkt;
    }

    /**
     * \ingroup diffserv
     * \brief Schedules the next packet to be dequeued based on the DRR algorithm.
     * \details This function iterates through the queues and checks if the packet size is less than or equal to the quantum.
     * If it is, the packet is dequeued and the counter is updated. The function continues until a packet is found or all queues are empty.
     * \returns A pointer to the next scheduled packet. If no packet is found, returns nullptr.
     * \note The function uses round-robin scheduling to ensure fair access to the queues.
     */
    Ptr<const Packet> DRR::Schedule() const
    {
        // get traffic class queues
        std::vector<TrafficClass*> queues = GetQueues();

        // check that there are queues to serve
        if (queues.size() == 0)
        {
            NS_LOG_UNCOND("No queues to serve.");
            return nullptr;
        }

        // Reset the next queue and quantum
        nextQueue = currentQueue;
        tempQueueQuantums = queueQuantums;

        // Check if the queues are empty
        uint32_t empty_count = 0;
        for (int i = 0; i < queues.size(); i++)
        {
            if (queues[i]->IsEmpty())
            {
                empty_count++;
            }
        }

        // If all queues are empty, return nullptr
        if (empty_count == queues.size())
        {
            NS_LOG_UNCOND("All queues are empty.");
            return nullptr;
        }

        // Iterate through the queues in a round-robin fashion
        // and check if the packet size is less than or equal to the quantum
        // If it is, dequeue the packet and update the quantum
        // If not, move to the next queue
        // and add the weight to the quantum
        while (true)
        {
            if (!queues[nextQueue]->IsEmpty())
            {
                // Set the quantum for the next queue to the sum of the current quantum and the weight of the queue
                tempQueueQuantums[nextQueue] = queues[nextQueue]->GetWeight() + tempQueueQuantums[nextQueue];

                // Get the packet size from the queue for the next queue
                uint32_t packet_size = queues[nextQueue]->Peek()->GetSize();

                // Check if the packet size is less than or equal to the quantum
                if (packet_size <= tempQueueQuantums[nextQueue])
                {
                    // Dequeue the packet and update the quantum
                    tempQueueQuantums[nextQueue] = tempQueueQuantums[nextQueue] - packet_size;

                    // Return a the the packet from the front of the queue
                    return queues[nextQueue]->Peek();
                }
            }

            // Set the next queue to the next one in the round-robin fashion
            nextQueue = (nextQueue + 1) % queues.size();
        }

        // If no packet is found, return nullptr
        NS_LOG_UNCOND("No packet found in the queues.");
        return nullptr;
    }

    /**
     * Adds a TrafficClass to q_class vector
     * \param trafficClass TrafficClass to add
     */
    void DRR::AddQueue(TrafficClass* trafficClass)
    {
        DiffServ::AddQueue(trafficClass);
        queueQuantums.push_back(0);
    }
} // namespace ns3
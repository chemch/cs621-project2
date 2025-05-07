#include "drr.h"
#include "ns3/log.h"
#include "diff-serv.h"
#include "ns3/packet.h"
#include "ns3/queue.h"

/**
     * DRR Algorithm Reference 1: https://en.wikipedia.org/wiki/Deficit_round_robin
     * This algorithm is a variant of the round-robin scheduling algorithm
     * that allows for fair bandwidth allocation among multiple queues.
     * It uses a quantum value to determine how much data can be sent from each queue
     * in each round. The quantum is adjusted based on the weight of the queue.
     * 
     * DRR Algorithm Reference 2: https://www.cs.columbia.edu/~hgs/teaching/2004/CSW4/lectures/drr.pdf
     * This paper provides a detailed explanation of the DRR algorithm,
     * including its implementation and performance analysis.
     * 
     * Shreedhar, M., & Varghese, G. (1995). Efficient Fair Queuing using Deficit Round Robin. 
     * In Proceedings of the Conference on Applications, Technologies, Architectures, and Protocols 
     * for Computer Communication (SIGCOMM ’95) (pp. 231–242). ACM. 
     * https://doi.org/10.1145/205287.205316
     * 
     * Also found this GitHub useful. It is old but provides a good reference for DRR: https://github.com/vilas897/Implementation-of-Deficit-Round-Robin-in-ns-3/blob/master/ns-3/src/traffic-control/model/drr-queue-disc.h
     */

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
            queueQuantum = tempDeficitCounter;

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
            queueQuantum = tempDeficitCounter;
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
     * 
     * Reference: https://en.wikipedia.org/wiki/Deficit_round_robin
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
        tempDeficitCounter = queueQuantum;

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
                tempDeficitCounter[nextQueue] = queues[nextQueue]->GetWeight() + tempDeficitCounter[nextQueue];

                // Get the packet size from the queue for the next queue
                uint32_t packet_size = queues[nextQueue]->Peek()->GetSize();

                // Check if the packet size is less than or equal to the quantum
                if (packet_size <= tempDeficitCounter[nextQueue])
                {
                    // Dequeue the packet and update the quantum
                    tempDeficitCounter[nextQueue] = tempDeficitCounter[nextQueue] - packet_size;

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
    void DRR::RegisterQueue(TrafficClass* trafficClass)
    {
        DiffServ::RegisterQueue(trafficClass);
        queueQuantum.push_back(0);
    }
} // namespace ns3
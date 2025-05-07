#include "spq.h"
#include <vector>
#include <limits>
#include <optional>
#include "ns3/log.h"

namespace ns3 {
    // Constant for debugging
    // This is used to enable or disable logging for the SPQ class
    static const bool SPQ_LOG_ENABLED = false;

    SPQ::SPQ() {}

    /**
     * \brief Retrieves the list of traffic classes.
     * \details This function returns the list of traffic classes associated with the DiffServ instance.
     * Note: This should really be a method in the  parent class DiffServ.
     */
    std::vector<TrafficClass*> SPQ::GetQueues() const
    {
        return q_class;
    }

    /**
     * \brief Schedule the next packet from the highest-priority non-empty queue.
     * \returns Ptr<const Packet> The packet at the head of the highest priority non-empty queue.
     * The trick here is that the best priority is actually the queue with the lowest priority level number.
     * This is because we are using strict priority scheduling, where lower numbers indicate higher priority.
     */
    Ptr<const Packet>
    SPQ::Schedule() const
    {
        // Get the list of queues
        auto const& queues = GetQueues();

        // Check if there are any queues available
        if (queues.empty())
        {
            NS_LOG_UNCOND("SPQ::Schedule: no queues configured");
            return nullptr;
        }

        // Start with the assumption that no queue is available
        // and the best priority is set to the maximum possible value
        std::optional<size_t> selectedQueue;
        int optimalPriority = std::numeric_limits<int>::max();

        // Iterate through the queues to find the one with the ideal priority
        for (size_t i = 0; i < queues.size(); ++i)
        {
            auto trafficClass = queues[i];

            // Check if the queue is not empty and has a higher priority than the current best (or optimal)
            if (!trafficClass->IsEmpty())
            {
                int trafficClassPriority = trafficClass->GetPriorityLevel();
                
                // If the current queue has a higher priority (lower number) than the best one found so far
                if (trafficClassPriority < optimalPriority)
                {
                    // Update the optimal priority and index
                    optimalPriority = trafficClassPriority;
                    selectedQueue = i;
                }
            }
        }

        // If no queue was found, return nullptr
        if (!selectedQueue.has_value())
        {
            NS_LOG_UNCOND("SPQ::Schedule: all queues empty");
            return nullptr;
        }

        // Finally, return the packet at the head of the best queue
        auto pkt = queues[*selectedQueue]->Peek();

        // If debug mode is enabled, log the selected queue and its properties
        if (SPQ_LOG_ENABLED)
        {
            NS_LOG_UNCOND("SPQ::Schedule: selected queue " << *selectedQueue
                        << " priority=" << optimalPriority
                        << " packetSize=" << (pkt ? pkt->GetSize() : 0));
        }

        // Return the packet at the head of the selected queue
        return pkt;
    }
} // namespace ns3
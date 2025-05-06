#include "spq.h"
#include "ns3/log.h"

namespace ns3 {

SPQ::SPQ() {}

/**
 * \brief Schedule the next packet from the highest-priority non-empty queue.
 * \returns Ptr<const Packet> The packet at the head of the highest priority non-empty queue.
 * The trick here is that the best priority is actually the queue with the lowest priority level number.
 * This is because we are using strict priority scheduling, where lower numbers indicate higher priority.
 * If all queues are empty, return nullptr.
 */
Ptr<const Packet>
SPQ::Schedule() const
{
    const std::vector<TrafficClass*> queues = GetQueues();

    // Check if there are any queues available
    if (queues.empty())
    {
        return nullptr;
    }

    // Start with the assumption that no queue is available
    // and the best priority is set to the maximum possible value
    uint32_t lowestPriorityLevelIndex = -1;
    int32_t lowestPriorityLevel = std::numeric_limits<int32_t>::max();

    // Iterate through the queues to find the one with the highest priority (lowest number)
    for (uint32_t i = 0; i < queues.size(); ++i)
    {
        // Check if the queue is not empty and has a higher priority than the current best
        // Note: In strict priority scheduling, lower numbers indicate higher priority
        if (!queues[i]->IsEmpty() && queues[i]->GetPriorityLevel() < lowestPriorityLevel)
        {
            // Update the best priority and index
            lowestPriorityLevel = queues[i]->GetPriorityLevel();
            lowestPriorityLevelIndex = i;
        }
    }

    // If no queue was found, return nullptr
    if (lowestPriorityLevelIndex == static_cast<uint32_t>(-1))
    {
        return nullptr;
    }

    // Finally, return the packet at the head of the best queue
    return queues[lowestPriorityLevelIndex]->Peek();
}

} // namespace ns3
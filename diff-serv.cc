#include "diff-serv.h"
#include "ns3/log.h"

namespace ns3 {
    /**
     * \brief Constructor for DiffServ class
     * \details Initializes the DiffServ class and sets up the logging component.
     */
    DiffServ::DiffServ() {}

    // Create Enqueue, Dequeue, Remove, and Peek methods
    /**
     * \brief Enqueues a packet into the queue.
     * \details This function adds a packet to the queue and returns true if successful.
     */
    bool DiffServ::Enqueue(Ptr<Packet> pkt) 
    {
        return DoEnqueue(pkt);
    }

    /**
     * \brief Dequeues a packet from the queue.
     * \details This function removes a packet from the queue and returns it.
     */
    Ptr<Packet> DiffServ::Dequeue()
    {
        return DoDequeue();
    }

    /**
     * \brief Peeks at the next packet in the queue without removing it.
     * \details This function returns a copy of the next packet in the queue without modifying the queue.
     */
    Ptr<const Packet> DiffServ::Peek() const
    {
        return DoPeek();
    }

    /**
     * \brief Removes a packet from the queue.
     * \details This function removes a packet from the queue and returns it.
     */
    Ptr<Packet> DiffServ::Remove()
    {
        return DoRemove();
    }

    /**
     * \brief Removes a packet from the queue.
     * \details This function removes a packet from the queue and returns it.
     */
    Ptr<Packet> DiffServ::DoRemove()
    {
        Ptr<const Packet> pktToRemove = Schedule();

        // If the scheduled packet is not null, classify it and remove from the corresponding queue
        if (pktToRemove != nullptr)
        {
            uint32_t index = Classify(pktToRemove->Copy());
            if (index < q_class.size())
            {
                return q_class[index]->Remove();
            }
        }

        return nullptr;
    }

    /**
     * \brief Enqueues a packet into the queue.
     * \details This function adds a packet to the queue and returns true if successful.
     */
    bool DiffServ::DoEnqueue(Ptr<Packet> pkt)
    {
        // Get the index of the queue to which the packet belongs
        uint32_t queueIndex = Classify(pkt);

        // If the index is invalid, return false
        if (queueIndex == static_cast<uint32_t>(-1) || queueIndex >= q_class.size())
        {
            NS_LOG_UNCOND("Invalid queue index. Packet not enqueued.");
            return false;
        }

        // Otherwise, enqueue the packet into the appropriate queue
        return q_class[queueIndex]->Enqueue(pkt);
    }


    /**
     * \brief Dequeues a packet from the queue.
     * \details This function removes a packet from the queue and returns it.
     */
    Ptr<Packet> DiffServ::DoDequeue()
    {
         // Get the next scheduled packet
        Ptr<const Packet> scheduledPkt = Schedule();

        // If the scheduledPkt packet is not null, classify it and dequeue from the corresponding queue
        if (scheduledPkt)
        {
            uint32_t queueIndex = Classify(scheduledPkt->Copy());

            // If the index is valid, dequeue from the corresponding queue
            if (queueIndex < q_class.size())
            {
                return q_class[queueIndex]->Dequeue();
            }
        }

        NS_LOG_UNCOND("No packet to dequeue. Returning nullptr.");
        return nullptr;
    }


    /**
     * \brief Peeks at the next packet in the queue without removing it.
     * \details This function returns a copy of the next packet in the queue without modifying the queue.
     */
    Ptr<const Packet> DiffServ::DoPeek() const
    {
         // Returns a packet to transmit
        Ptr<const Packet> scheduledPkt = Schedule();

        // If the scheduled packet is not null, return a copy of it
        return scheduledPkt != nullptr ? scheduledPkt->Copy() : nullptr;
    }

    /**
     * \brief Classifies a packet based on its traffic class.
     * \details This function iterates through the list of traffic classes and checks if the packet matches any of them.
     */
    uint32_t DiffServ::Classify(Ptr<Packet> pkt)
    {
        // Set the default index to an invalid value
        constexpr uint32_t NO_QUEUE = std::numeric_limits<uint32_t>::max();
        uint32_t defaultIndex = NO_QUEUE;

        for (uint32_t i = 0; i < q_class.size(); ++i)
        {
            // Check if the packet matches the traffic class
            // If it does, return the index of the traffic class
            if (q_class[i]->Match(pkt))
            {
                return i;
            }

            // If the packet doesn't match any class, check if it's the default class
            if (q_class[i]->GetIsDefault())
            {
                defaultIndex = i;
            }
        }

        NS_LOG_UNCOND("Falling back to default queue: " << defaultIndex);
        return defaultIndex;
    }

    /**
     * \brief Add a traffic class (or queue) to the DiffServ instance.
     */
    void DiffServ::AddQueue(TrafficClass* trafficClass)
    {
        q_class.push_back(trafficClass);
    }
} // namespace ns3
#include "diff-serv.h"
#include "ns3/log.h"

namespace ns3 {

/**
 * * \brief Constructor for DiffServ class
 * * \details Initializes the DiffServ class and sets up the logging component.
 */
DiffServ::DiffServ() {}

/**
 * * \brief Removes a packet from the queue.
 * * \details This function removes a packet from the queue and returns it.
 */
Ptr<Packet> DiffServ::Remove()
{
    return ExecuteRemove();
}

/**
 * * \brief Peeks at the next packet in the queue without removing it.
 * * \details This function returns a copy of the next packet in the queue without modifying the queue.
 */
Ptr<const Packet> DiffServ::ExecutePeek() const
{
    // Returns a packet to transmit
    Ptr<const Packet> scheduledPkt = Schedule();

    return scheduledPkt != nullptr ? scheduledPkt->Copy() : nullptr;
}

/**
 * * \brief Constructor for DiffServ class
 * * \details Sets up the DiffServ class.
 */
bool DiffServ::Enqueue(Ptr<Packet> pkt)
{
    return ExecuteEnqueue(pkt);
}


Ptr<Packet> DiffServ::Dequeue()
{
    return ExecuteDequeue();
}


/**
 * * \brief Peeks at the next packet in the queue without removing it.
 * * \details This function returns a copy of the next packet in the queue without modifying the queue.
 */
Ptr<const Packet> DiffServ::Peek() const
{
    return ExecutePeek();
}


/**
 * * \brief Classifies a packet based on its traffic class.
 * * \details This function iterates through the list of traffic classes and checks if the packet matches any of them.
 */
uint32_t DiffServ::Classify(Ptr<Packet> pkt)
{
    // effectively, this is max uint32_t
    uint32_t default_index = -1;

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
            default_index = i;
        }
    }

    return default_index;
}

/**
 * * \brief Add a traffic class (or queue) to the DiffServ instance.
 */
void DiffServ::AddQueue(TrafficClass* trafficClass)
{
    q_class.push_back(trafficClass);
}

/**
 * * \brief Retrieves the list of traffic classes.
 * * \details This function returns the list of traffic classes associated with the DiffServ instance.
 */
std::vector<TrafficClass*> DiffServ::GetQueues() const
{
    return q_class;
}

/**
 * * \brief Enqueues a packet into the appropriate queue based on its classification.
 * * \details This function classifies the packet and enqueues it into the corresponding queue.
 */
bool DiffServ::ExecuteEnqueue(Ptr<Packet> pkt)
{

    // Get the index of the queue to which the packet belongs
    uint32_t index = Classify(pkt);

    // If the index is invalid, return false
    if (index == static_cast<uint32_t>(-1) || index >= q_class.size())
    {
        return false;
    }

    // Print the queue index
    // std::cout << "Match Found: Queue index to Enqueue Into: " << index << std::endl;

    // Otherwise, enqueue the packet into the appropriate queue
    return q_class[index]->Enqueue(pkt);
}

/** 
 * * \brief Dequeues a packet from the appropriate queue based on its classification.
 */
Ptr<Packet> DiffServ::ExecuteDequeue()
{
    // Get the next scheduled packet
    Ptr<const Packet> scheduledPkt = Schedule();

    // If the scheduledPkt packet is not null, classify it and dequeue from the corresponding queue
    if (scheduledPkt)
    {
        uint32_t index = Classify(scheduledPkt->Copy());

        // print queue index
        // std::cout << "Queue index to Dequeue From: " << index << std::endl;

        if (index < q_class.size())
        {
            return q_class[index]->Dequeue();
        }
    }

    return nullptr;
}

/**
 * * \brief Removes a packet from the queue.
 * * \details This function removes a packet from the queue and returns it.
 */
Ptr<Packet> DiffServ::ExecuteRemove()
{
    Ptr<const Packet> pktToRemove = Schedule();

    // If the scheduled packet is not null, classify it and remove from the corresponding queue
    if (pktToRemove != nullptr)
    {
        uint32_t index = Classify(pktToRemove->Copy());
        if (index < q_class.size())
        {
            return q_class[index]->Dequeue();
        }
    }

    return nullptr;
}

} // namespace ns3
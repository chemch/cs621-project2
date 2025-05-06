#include "ns3/core-module.h"
#include "ns3/log.h"
#include "filter.h"
#include "traffic-class.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("TrafficClass");

/**
 * \ingroup diffserv
 * \brief Constructor for TrafficClass.
 *
 * Initializes the traffic class with default values.
 */
TrafficClass::TrafficClass() : m_packets(0), m_maxPackets(100), m_weight(0), m_priorityLevel(0), m_isDefault(false) {}

/**
 * \ingroup diffserv
 * \brief Getter for is default
 */
bool TrafficClass::GetIsDefault() const
{
    NS_LOG_FUNCTION(this);
    return m_isDefault;
}

uint32_t TrafficClass::GetPackets() const
{
    return m_packets;
}

/**
 * \ingroup diffserv
 * \brief Setter for is default
 */
void TrafficClass::SetIsDefault(bool default_queue)
{
    NS_LOG_FUNCTION(this);
    m_isDefault = default_queue;
}

/** 
 * \ingroup diffserv
 * \brief Enqueues a packet into the traffic class.
 */
bool TrafficClass::Enqueue(Ptr<Packet> pkt)
{
    if (m_packets < m_maxPackets)
    {
        m_queue.push(pkt);
        m_packets++;

        // std::cout << "[TrafficClass::Enqueue] Packet enqueued. "
        //           << "Current packet count: " << m_packets << std::endl;

        return true;
    }

    std::cout << "[TrafficClass::Enqueue] Packet dropped. Queue full." << std::endl;
    return false;
}

/**
 * \ingroup diffserv
 * \brief Dequeues a packet from the traffic class.
 */
Ptr<Packet> TrafficClass::Dequeue()
{
    // Return null pointer on empty queue
    if (m_queue.empty())
    {
        std::cout << "Be aware that the queue is empty." << std::endl;
        return nullptr;
    }

    // Dequeue the packet and update the packet count
    Ptr<Packet> pkt = m_queue.front();

    // Pop the packet from the queue and decrement the packet count
    m_queue.pop();
    m_packets--;
    
    return pkt;
}

/** 
 * \ingroup diffserv
 * \brief Checks if there are no packets.
 */
bool TrafficClass::IsEmpty() const
{
    return m_packets == 0;
}

/**
 * Getter for m_queue's size
 * \returns number of packets in m_queues
 */
uint32_t
TrafficClass::GetSize()
{
    return m_queue.size();
}

/**
 * \ingroup diffserv
 * \brief Getter for the list of filter pointers.
 */
std::vector<Filter*> TrafficClass::GetFilters() const
{
    return m_filters;
}

/**
 * \ingroup diffserv
 * \brief Setter for replacing the entire list of filters.
 */
void TrafficClass::SetFilters(const std::vector<Filter*>& filters)
{
    NS_LOG_FUNCTION(this);

    // Clear the existing filters
    m_filters.clear();

    // Add the new filters
    m_filters = filters;
}

/**
 * \ingroup diffserv
 * \brief Getter for maximum number of packets allowed in queue.
 */
uint32_t
TrafficClass::GetMaxPackets() const
{
    NS_LOG_FUNCTION(this);
    return m_maxPackets;
}

/**
 * \ingroup diffserv
 * \brief Checks front packet on the queue.
 */
Ptr<Packet> TrafficClass::Peek() const
{

    // Return null pointer on empty queue
    if (IsEmpty())
    {
        return nullptr;
    }

    // Else, return the front packet
    Ptr<Packet> pkt = m_queue.front();
    return pkt;
}

/**
 * \ingroup diffserv
 * \brief Add a singular filter
 */
void TrafficClass::AddFilter(Filter *filter)
{
    NS_LOG_FUNCTION(this);
    m_filters.push_back(filter);
}

/** 
 * \ingroup diffserv
 * \brief Setter for max number of packets
 */
void TrafficClass::SetMaxPackets(uint32_t max)
{
    NS_LOG_FUNCTION(this);
    m_maxPackets = max;
}

/**
 * \ingroup diffserv
 * \brief Setter for weight
 */
void TrafficClass::SetWeight(double weight)
{
    NS_LOG_FUNCTION(this);
    m_weight = weight;
}

/**
 * \ingroup diffserv
 * \brief Getter for weight
 */
double TrafficClass::GetWeight() const
{
    NS_LOG_FUNCTION(this);
    return m_weight;
}

/** 
 * \ingroup diffserv
 * \brief Setter for priority level
 */
void TrafficClass::SetPriorityLevel(uint32_t lvl)
{
    m_priorityLevel = lvl;
}

/** 
 * \ingroup diffserv
 * \brief Getter for priority level
 */
uint32_t TrafficClass::GetPriorityLevel() const
{
    NS_LOG_FUNCTION(this);
    return m_priorityLevel;
}

/** 
 * \ingroup diffserv
 * \brief Matches a packet to each Filter in filters vector.
 */
bool TrafficClass::Match(Ptr<Packet> pkt) const
{
    // If not filters, always match
    if (m_filters.empty())
    {
        return true;
    }

    // Else, check each filter
    for (Filter *filter : m_filters)
    {
        if (filter->Match(pkt))
        {
            return true;
        }
    }

    return false;
}
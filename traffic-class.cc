#include "ns3/core-module.h"
#include "ns3/log.h"
#include "filter.h"
#include "traffic-class.h"

namespace ns3 {
    /**
     * \ingroup diffserv
     * \brief Constructor for TrafficClass.
     *
     * Initializes the traffic class with default values.
     */
    TrafficClass::TrafficClass(): m_packets(0), m_maxPackets(100), m_weight(0), m_priorityLevel(0), m_isDefault(false) {}

    /**
     * \ingroup diffserv
     * \brief Getter for is default
     */
    bool TrafficClass::GetIsDefault() const
    {
        return m_isDefault;
    }

    /**
     * \ingroup diffserv
     * \brief Setter for is default
     */
    void TrafficClass::SetIsDefault(bool default_queue)
    {
        m_isDefault = default_queue;
    }

    /** 
     * \ingroup diffserv
     * \brief Enqueues a packet into the traffic class.
     */
    bool TrafficClass::Enqueue(Ptr<Packet> pkt)
    {
        // Add the packet to the queue if it is not full
        if (m_packets < m_maxPackets)
        {
            m_queue.push(pkt);
            m_packets++;

            return true;
        }

        return false;
    }

    /**
     * \ingroup diffserv
     * \brief Removes a packet from the traffic class.
     * \details Pops the next packet but does *not* fire the "served" trace logging.
     * From my research this is the main difference between Dequeue() and Remove().
     */
    Ptr<Packet> TrafficClass::Remove()
    {
        // Return null pointer on empty queue
        if (m_queue.empty())
        {
            return nullptr;
        }

        // Remove the packet and update the packet count
        Ptr<Packet> pkt = m_queue.front();
        
        // Pop the packet from the queue and decrement the packet count
        m_queue.pop();
        m_packets--;
        
        return pkt;
    }

    /**
     * \ingroup diffserv
     * \brief Dequeues a packet from the traffic class.
     * \details Pops the next packet and fires the "served" trace callbacks.
     */
    Ptr<Packet> TrafficClass::Dequeue()
    {
        // Use the silent Remove() to pop the packet
        Ptr<Packet> pkt = Remove();
        
        // Return null pointer if the queue is empty
        if (!pkt)
        {
            return nullptr;
        }
        
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
     * \ingroup diffserv
     * \brief Getter for maximum number of packets allowed in queue.
     */
    uint32_t
    TrafficClass::GetMaxPackets() const
    {
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
        m_filters.push_back(filter);
    }

    /** 
     * \ingroup diffserv
     * \brief Setter for max number of packets
     */
    void TrafficClass::SetMaxPackets(uint32_t max)
    {
        m_maxPackets = max;
    }

    /**
     * \ingroup diffserv
     * \brief Setter for weight
     */
    void TrafficClass::SetWeight(double weight)
    {
        m_weight = weight;
    }

    /**
     * \ingroup diffserv
     * \brief Getter for weight
     */
    double TrafficClass::GetWeight() const
    {
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
            // If the filter matches, return true
            if (filter->Match(pkt))
            {
                return true;
            }
        }

        return false;
    }
} // namespace ns3
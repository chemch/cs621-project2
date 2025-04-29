#ifndef TRAFFIC_CLASS_H
#define TRAFFIC_CLASS_H

#include "ns3/packet.h"
#include <queue>
#include <vector>
#include <string>
#include "filter.h"


namespace ns3 {

/**
 * \ingroup diffserv
 * \brief TrafficClass class to manage packets and filters.
 *
 * This class allows setting and getting various properties of a traffic class,
 * including name, packets, weight, priority level, default status, and filters.
 */
class TrafficClass
{
public:
    TrafficClass();

    /**
     * Default status of the traffic class.
     * If true, this traffic class is the default one.
     */
    void SetIsDefault(bool isDefault);
    bool GetIsDefault() const;

    /**
     * Packet Count in traffic class.
     */
    uint32_t GetPackets() const;

    /**
     * Maximum number of packets in the traffic class.
     */
    void SetMaxPackets(uint32_t num);
    uint32_t GetMaxPackets() const;

    /**
     * Weight of the traffic class.
     */
    void SetWeight(double weight);
    double GetWeight() const;

    /**
     * Priority level of the traffic class.
     */
    void SetPriorityLevel(uint32_t level);
    uint32_t GetPriorityLevel() const;

    /** 
     * Filters -
     * Set, Get or Add filters to the traffic class.
     */
    std::vector<Filter*> GetFilters() const;
    void SetFilters(const std::vector<Filter*>& filters);
    void AddFilter(Filter* filter);

    /** 
     * Queue Operations - Important!
     */
    bool Enqueue(Ptr<Packet> pkt);
    Ptr<Packet> Dequeue();
    Ptr<Packet> Peek() const;
    bool IsEmpty() const;

    /**
     * Check if the packet matches the filters.
     */
    bool Match(Ptr<Packet> pkt) const;

private:
    uint32_t m_packets;
    uint32_t m_maxPackets;
    double m_weight;
    uint32_t m_priorityLevel;
    bool m_isDefault;

    // Queue and Filters most important
    std::queue<Ptr<Packet>> m_queue;
    std::vector<Filter*> m_filters;
};

} // namespace ns3

#endif // TRAFFIC_CLASS_H
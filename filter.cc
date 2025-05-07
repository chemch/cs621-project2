#include "ns3/packet.h"
#include "ns3/ipv4-header.h"
#include "ns3/log.h"
#include <string>
#include <vector>
#include "filter-element.h"
#include "filter.h"

NS_LOG_COMPONENT_DEFINE("Filter");  

namespace ns3 {
    /**
     * \ingroup diffserv
     * \brief Constructor for Filter.
     */
    Filter::Filter() {}

    /**
     * \ingroup diffserv
     * \brief Destructor for Filter.
     */
    Filter::~Filter() {}

    /**
     * Adds a filter element to the filter.
     * @param filterElement
     */
    void Filter::AddFilterElement(FilterElement* filterElement) {
        m_filterElements.push_back(filterElement);
    }

    /**
     * Matches a packet against all (every single) filter elements.
     * @param packet The packet to check.
     * @return true if the packet matches all filter elements.
     */
    bool Filter::Match(Ptr<Packet> packet) {
        for (auto filterElement : m_filterElements) {

            // Check if the filter element matches the packet
            if (!filterElement->Match(packet)) {

                NS_LOG_INFO("Filter::Match: Packet does not match filter element.");
                return false;
            }
        }

        NS_LOG_INFO("Filter::Match: Packet matches all filter elements.");
        return true;
    }
} // namespace ns3
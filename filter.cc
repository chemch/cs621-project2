#include "ns3/packet.h"
#include "ns3/ipv4-header.h"
#include "ns3/log.h"
#include <string>
#include <vector>
#include "filter-element.h"
#include "filter.h"

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
     * Returns all filter elements.
     * @return A vector of pointers to the filter elements.
     */
    std::vector<FilterElement*> Filter::getFilterElements() {
        return m_filterElements;
    }

    /**
     * Adds multiple filter elements to the filter.
     * @param filterElements
     */
    void Filter::addAllFilterElements(std::vector<FilterElement*> filterElements) {
        for (auto filterElement : filterElements) {
            m_filterElements.push_back(filterElement);
        }
    }

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
                // std::cout << "[Filter::match] Packet failed filter." << std::endl;
                return false;
            }
        }

        // std::cout << "[Filter::match] Packet passed filter." << std::endl;
        return true;
    }
} // namespace ns3
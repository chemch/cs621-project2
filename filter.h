#ifndef FILTER_H
#define FILTER_H

#include "ns3/packet.h"
#include <vector>
#include <memory>
#include "filter-element.h"

namespace ns3 {

/**
 * \ingroup diffserv
 * \brief Filter class to manage multiple filter elements.
 *
 * This class allows adding multiple filter elements and checks if a packet
 * matches any of the added filter elements.
 */
class Filter
{
public:
    /**
     * \brief Constructor.
     */
    Filter();

    /**
     * \brief Destructor.
     */
    ~Filter();

    /**
     * \brief Add a filter element to the filter.
     * \param filterElement The filter element to add.
     */
    void AddFilterElement(FilterElement* filterElement);

    /**
     * \brief Add multiple filter elements to the filter.
     * \param filterElements A vector of filter elements to add.
     */
    void addAllFilterElements(std::vector<FilterElement*> filterElements);

    /**
     * \brief Check if a packet matches the filter elements.
     * \param packet The packet to check.
     * \return true if the packet matches all filter elements, false if not.
     */
    bool Match(Ptr<Packet> packet);

    /**
     * \brief Get all filter elements.
     * \return A vector of pointers to the filter elements.
     */
    std::vector<FilterElement*> getFilterElements();

private:
    std::vector<FilterElement*> m_filterElements;
};

} // namespace ns3

#endif // FILTER_H
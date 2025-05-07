#ifndef FILTER_ELEMENT_H
#define FILTER_ELEMENT_H

#include "ns3/packet.h"
#include "ns3/object.h"

namespace ns3 {

    /**
     * \ingroup diffserv
     * \brief Abstract base class for filter elements.
     */    
    class FilterElement : public Object
    {
        public:
            /**
             * \brief Decide if there is a packet against the filter element.
             * \param pkt The packet to evaluate.
             * \return true if the packet matches, false otherwise.
             */
            virtual bool Match(Ptr<Packet> pkt) const = 0;

            /**
             * \brief Virtual Destructor. Must be overriden. 
             */
            virtual ~FilterElement() = default;
    };
} // namespace ns3

#endif // FILTER_ELEMENT_H

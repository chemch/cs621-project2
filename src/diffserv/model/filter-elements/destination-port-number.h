#ifndef DESTINATION_PORT_NUMBER_H
#define DESTINATION_PORT_NUMBER_H

#include "ns3/packet.h"
#include "ns3/object.h"
#include "filter-elements/filter-element.h"

namespace ns3 {

/**
 * \ingroup diffserv
 * \brief Class to match the destination port number in a packet's L4 (Transport) header.
 */
class DestinationPortNumber : public FilterElement
{

public:
    /**
     * \brief Constructor.
     * \param destinationPort The destination port number to match.
     */
    DestinationPortNumber(uint16_t destinationPort);

    /**
     * \brief Match the packet against the referenced destination port number.
     * \param pkt The packet to inspect.
     * \returns true if the destination port matches, false otherwise.
     */
    bool Match(Ptr<Packet> pkt) const override;

private:
    uint16_t m_destinationPort;
};

} // namespace ns3

#endif // DESTINATION_PORT_NUMBER_H
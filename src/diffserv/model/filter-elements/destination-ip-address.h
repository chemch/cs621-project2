#ifndef DESTINATION_IP_ADDRESS_H
#define DESTINATION_IP_ADDRESS_H

#include "ns3/packet.h"
#include "ns3/ipv4-address.h"
#include "filter-elements/filter-element.h"

namespace ns3 {

/**
 * \ingroup diffserv
 * \brief Class to match destination IP address.
 */
class DestinationIPAddress : public FilterElement
{

public:
    /**
     * \brief Constructor.
     * \param destinationIp The destination IP address to match.
     */
    DestinationIPAddress(Ipv4Address destinationIp);

    /**
     * \brief Match the packet against the destination IP address.
     * \param pkt The packet to check.
     * \returns true if the packet matches, false otherwise.
     */
    bool Match(Ptr<Packet> pkt) const override;

private:
    Ipv4Address m_destinationIp;
};

} // namespace ns3

#endif // DESTINATION_IP_ADDRESS_H
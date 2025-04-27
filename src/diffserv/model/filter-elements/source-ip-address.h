#ifndef SOURCE_IP_ADDRESS_H
#define SOURCE_IP_ADDRESS_H

#include "ns3/packet.h"
#include "ns3/ipv4-address.h"
#include "filter-elements/filter-element.h"

namespace ns3 {

/**
 * \ingroup diffserv
 * \brief Class to match source IP address.
 */
class SourceIPAddress : public FilterElement
{

public:
    /**
     * \brief Constructor.
     * \param sourceIp The source IP address to match.
     */
    SourceIPAddress(Ipv4Address sourceIp);

    /**
     * \brief Match the packet against the source IP address.
     * \param pkt The packet to check.
     * \return true if the packet matches, false otherwise.
     */
    bool Match(Ptr<Packet> pkt) const override;

private:
    Ipv4Address m_sourceIp;
};

} // namespace ns3

#endif // SOURCE_IP_ADDRESS_H
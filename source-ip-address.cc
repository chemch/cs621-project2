#include "ns3/ipv4-header.h"
#include "source-ip-address.h"

namespace ns3 {

/**
 * \ingroup diffserv
 * \brief Constructor for SourceIPAddress.
 * \param sourceIp The source IP address to match.
 */
SourceIPAddress::SourceIPAddress(Ipv4Address sourceIp)
  : m_sourceIp(sourceIp) {}

/**
 * \ingroup diffserv
 * \brief Match the packet against the source IP address.
 * \param pkt The packet to check.
 * \return true if the packet matches, false otherwise.
 */
bool
SourceIPAddress::Match(Ptr<Packet> pkt) const
{
    Ipv4Header ipv4Header;
    Ptr<Packet> packetCopy = pkt->Copy();

    // If we can't access the header, return false
    if (!packetCopy->PeekHeader(ipv4Header))
    {
        return false;
    }

    return ipv4Header.GetSource() == m_sourceIp;
}

} // namespace ns3
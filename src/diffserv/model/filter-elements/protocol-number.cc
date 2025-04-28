#include "ns3/ipv4-header.h"
#include "protocol-number.h"

namespace ns3 {

/**
 * \ingroup diffserv
 * \brief Constructor for ProtocolNumber.
 * \param protocolNumber The protocol number to match.
 */
ProtocolNumber::ProtocolNumber(uint8_t protocolNumber)
  : m_protocolNumber(protocolNumber) {}

/**
 * \ingroup diffserv
 * \brief Match the packet against the stored protocol number.
 * \param pkt The packet to inspect.
 * \returns true if the protocol number matches, false otherwise.
 */
bool
ProtocolNumber::Match(Ptr<Packet> pkt) const
{
    // Make a copy of the packet first to simplify header access
    Ipv4Header ipv4Header;
    Ptr<Packet> packetCopy = pkt->Copy();

    // If we can't peek at the IPv4 header, the packet doesn't match.
    if (!packetCopy->PeekHeader(ipv4Header))
    {
        return false;
    }

    // Compare protocol number from the header
    return ipv4Header.GetProtocol() == m_protocolNumber;
}

} // namespace ns3
#include "ns3/ipv4-header.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "source-port-number.h"

namespace ns3 {

/**
 * \ingroup diffserv
 * \brief Constructor for SourcePortNumber.
 * \param sourcePort The source port number to match.
 */
SourcePortNumber::SourcePortNumber(uint16_t sourcePort)
  : m_sourcePort(sourcePort) {}

/**
 * \ingroup diffserv
 * \brief Match the packet against the stored source port number.
 * \param pkt The packet to inspect.
 * \returns true if the source port matches, else false.
 */
bool
SourcePortNumber::Match(Ptr<Packet> pkt) const
{
    // Make a copy of the packet to avoid modifying the original
    Ipv4Header ipv4Header;
    Ptr<Packet> packetCopy = pkt->Copy();

    // If we're unable to remove the IPv4 header, return false
    if (!packetCopy->RemoveHeader(ipv4Header))
    {
        return false;
    }

    uint8_t protocol = ipv4Header.GetProtocol();

    // If TCP, check TCP header
    if (protocol == 6)
    {
        // TCP protocol number is 6
        TcpHeader tcpHeader;

        // Ensure we can peek the TCP header
        if (!packetCopy->PeekHeader(tcpHeader))
        {
            return false;
        }

        // Check if the source port in the TCP header matches
        return tcpHeader.GetSourcePort() == m_sourcePort;
    }
    // If UDP, check the UDP Header instead
    else if (protocol == 17)
    {
        // UDP protocol number is 17
        UdpHeader udpHeader;

        // Ensure we can peek the UDP header
        if (!packetCopy->PeekHeader(udpHeader))
        {
            return false;
        }

        // Check if the source port in the UDP header matches
        return udpHeader.GetSourcePort() == m_sourcePort;
    }

    // If not TCP or UDP, no match
    return false;
}

} // namespace ns3
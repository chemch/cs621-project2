#include "ns3/ipv4-header.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "destination-port-number.h"

namespace ns3 {

/**
 * \ingroup diffserv
 * \brief Constructor for DestinationPortNumber.
 * \param destinationPort The destination port number to match.
 */
DestinationPortNumber::DestinationPortNumber(uint16_t destinationPort)
  : m_destinationPort(destinationPort) {}

/**
 * \ingroup diffserv
 * \brief Match the packet against the stored destination port number.
 * \param pkt The packet to inspect.
 * \returns true if the destination port matches, else false.
 */
bool
DestinationPortNumber::Match(Ptr<Packet> pkt) const
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

        // Check if the destination port in the TCP header matches
        return tcpHeader.GetDestinationPort() == m_destinationPort;
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

        // Check if the destination port in the UDP header matches
        return udpHeader.GetDestinationPort() == m_destinationPort;
    }

    // If not TCP or UDP, no match
    return false;
}

} // namespace ns3
#include "ns3/ipv4-header.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "destination-port-number.h"
#include "ns3/ppp-header.h"

namespace ns3 {

/**
 * \ingroup diffserv
 * \brief Constructor for DestinationPortNumber.
 * \param destinationPort The destination port number to match.
 */
DestinationPortNumber::DestinationPortNumber(uint32_t destinationPort)
  : m_destinationPort(destinationPort) {}

/**
 * \ingroup diffserv
 * \brief Match the packet against the stored destination port number.
 * \param pkt The packet to inspect.
 * \returns true if the destination port matches, else false.
 */
bool DestinationPortNumber::Match(Ptr<Packet> pkt) const
{
    Ptr<Packet> packetCopy = pkt->Copy();
    PppHeader pppHeader;

    if (!packetCopy->RemoveHeader(pppHeader))
    {
        std::cout << "[PortMatch] Failed to remove PPP header" << std::endl;
        return false;
    }

    Ipv4Header ipv4Header;
    if (!packetCopy->RemoveHeader(ipv4Header))
    {
        std::cout << "[PortMatch] Failed to remove IPv4 header" << std::endl;
        return false;
    }

    uint8_t protocol = ipv4Header.GetProtocol();
    // std::cout << "[PortMatch] Protocol = " << static_cast<int>(protocol) << std::endl;

    if (protocol == 6)
    {
        TcpHeader tcpHeader;
        if (!packetCopy->PeekHeader(tcpHeader))
        {
            std::cout << "[PortMatch] Failed to peek TCP header" << std::endl;
            return false;
        }
        uint16_t dst = tcpHeader.GetDestinationPort();
        // std::cout << "[PortMatch] TCP dst port: " << dst
        //           << " | expected: " << m_destinationPort << std::endl;
        return dst == m_destinationPort;
    }
    else if (protocol == 17)
    {
        UdpHeader udpHeader;
        if (!packetCopy->PeekHeader(udpHeader))
        {
            std::cout << "[PortMatch] Failed to peek UDP header" << std::endl;
            return false;
        }
        uint16_t dst = udpHeader.GetDestinationPort();
        // std::cout << "[PortMatch] UDP dst port: " << dst
        //           << " | expected: " << m_destinationPort << std::endl;
        return dst == m_destinationPort;
    }

    std::cout << "[PortMatch] Unknown protocol: " << static_cast<int>(protocol) << std::endl;
    return false;
}

} // namespace ns3
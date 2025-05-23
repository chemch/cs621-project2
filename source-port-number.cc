#include "ns3/ipv4-header.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "source-port-number.h"
#include "ns3/ppp-header.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("SourcePortNumber");  

namespace ns3 {

    /**
     * \ingroup diffserv
     * \brief Constructor for SourcePortNumber.
     * \param sourcePort The source port number to match.
     */
    SourcePortNumber::SourcePortNumber(uint32_t sourcePort): m_sourcePort(sourcePort) {}

    /**
     * \ingroup diffserv
     * \brief Match the packet against the stored source port number.
     * \param pkt The packet to inspect.
     * \returns true if the source port matches, else false.
     */
    bool SourcePortNumber::Match(Ptr<Packet> pkt) const
    {
        // Make a copy of the packet to avoid modifying the original
        Ptr<Packet> packetCopy = pkt->Copy();
        
        // Define the headers we need to remove
        Ipv4Header ipv4Header;
        PppHeader pppHeader;

        if (!packetCopy->RemoveHeader(pppHeader))
        {
            NS_LOG_WARN("Packet does not contain PPP header");
            return false;
        }

        // If we're unable to remove the IPv4 header, return false
        if (!packetCopy->RemoveHeader(ipv4Header))
        {
            NS_LOG_WARN("Packet does not contain IPv4 header");
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
                NS_LOG_WARN("Packet does not contain TCP header");
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
                NS_LOG_WARN("Packet does not contain UDP header");
                return false;
            }

            // Check if the source port in the UDP header matches
            return udpHeader.GetSourcePort() == m_sourcePort;
        }

        // If not TCP or UDP, no match
        return false;
    }
} // namespace ns3
#include "ns3/ipv4-header.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "destination-port-number.h"
#include "ns3/ppp-header.h"
#include "ns3/log.h"

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE("DestinationPortNumber"); 

    /**
     * \ingroup diffserv
     * \brief Constructor for DestinationPortNumber.
     * \param destinationPort The destination port number to match.
     */
    DestinationPortNumber::DestinationPortNumber(uint32_t destinationPort): m_destinationPort(destinationPort) {}

    /**
     * \ingroup diffserv
     * \brief Match the packet against the stored destination port number.
     * \param pkt The packet to inspect.
     * \returns true if the destination port matches, else false.
     */
    bool DestinationPortNumber::Match(Ptr<Packet> pkt) const
    {
        // Make a copy of the packet to avoid modifying the original
        Ptr<Packet> packetCopy = pkt->Copy();

        // Use Headers to remove the PPP header and check the destination port
        Ipv4Header ipv4Header;
        PppHeader pppHeader;

        if (!packetCopy->RemoveHeader(pppHeader))
        {
            NS_LOG_WARN("Packet does not contain PPP header");
            return false;
        }
        
        if (!packetCopy->RemoveHeader(ipv4Header))
        {
            NS_LOG_WARN("Packet does not contain IPv4 header");
            return false;
        }

        // Check the protocol type (TCP or UDP)
        uint8_t protocol = ipv4Header.GetProtocol();

        // Check if the protocol is TCP (6) or UDP (17)
        if (protocol == 6)
        {
            TcpHeader tcpHeader;
            if (!packetCopy->PeekHeader(tcpHeader))
            {
                NS_LOG_WARN("Packet does not contain TCP header");
                return false;
            }

            uint16_t dst = tcpHeader.GetDestinationPort();
            return dst == m_destinationPort;
        }
        else if (protocol == 17)
        {
            UdpHeader udpHeader;
            if (!packetCopy->PeekHeader(udpHeader))
            {
                NS_LOG_WARN("Packet does not contain UDP header");
                return false;
            }

            uint16_t dst = udpHeader.GetDestinationPort();
            return dst == m_destinationPort;
        }

        NS_LOG_WARN("Unknown protocol: " << static_cast<int>(protocol));
        return false;
    }
} // namespace ns3
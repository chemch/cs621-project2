#include "ns3/ipv4-header.h"
#include "source-ip-address.h"
#include "ns3/ppp-header.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("SourceIPAddress");  

namespace ns3 {
    /**
     * \ingroup diffserv
     * \brief Constructor for SourceIPAddress.
     * \param sourceIp The source IP address to match.
     */
    SourceIPAddress::SourceIPAddress(Ipv4Address sourceIp): m_sourceIp(sourceIp) {}

    /**
     * \ingroup diffserv
     * \brief Match the packet against the source IP address.
     * \param pkt The packet to check.
     * \return true if the packet matches, false otherwise.
     */
    bool
    SourceIPAddress::Match(Ptr<Packet> pkt) const
    {
        // Make a copy of the packet to avoid modifying the original
        Ptr<Packet> packetCopy = pkt->Copy();

        // Use Headers to remove the PPP header and check the source IP
        Ipv4Header ipv4Header;
        PppHeader pppHeader;

        if (!packetCopy->RemoveHeader(pppHeader))
        {
            NS_LOG_WARN("Packet does not contain PPP header");
            return false;
        }

        // If we can't access the header, return false
        if (!packetCopy->PeekHeader(ipv4Header))
        {
            NS_LOG_WARN("Packet does not contain IPv4 header");
            return false;
        }

        // Check if the source IP address matches
        return ipv4Header.GetSource() == m_sourceIp;
    }
} // namespace ns3
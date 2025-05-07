#include "ns3/ipv4-header.h"
#include "destination-ip-address.h"
#include "ns3/ppp-header.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("DestinationIPAddress");  

namespace ns3 {
    /**
     * \ingroup diffserv
     * \brief Constructor for DestinationIPAddress.
     * \param destinationIp The destination IP address to match.
     */
    DestinationIPAddress::DestinationIPAddress(Ipv4Address destinationIp): m_destinationIp(destinationIp) {}

    /**
     * \ingroup diffserv
     * \brief Match the packet against the destination IP address.
     * \param pkt The packet to check.
     * \returns true if the packet matches, false otherwise.
     */
    bool
    DestinationIPAddress::Match(Ptr<Packet> pkt) const
    {
        // Make a copy of the packet to avoid modifying the original
        Ptr<Packet> packetCopy = pkt->Copy();

        // Use Headers to remove the PPP header and check the destination IP
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

        return ipv4Header.GetDestination() == m_destinationIp;
    }

} // namespace ns3
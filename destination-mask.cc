#include "ns3/ipv4-header.h"
#include "destination-mask.h"
#include "ns3/ppp-header.h"
#include "ns3/log.h"

namespace ns3 {
    NS_LOG_COMPONENT_DEFINE("DestinationMask");  

    /**
     * \ingroup diffserv
     * \brief Constructor for DestinationMask.
     * \param mask The destination IP mask to apply.
     * \param ipAddress The destination IP address to match.
     */
    DestinationMask::DestinationMask(Ipv4Mask mask, Ipv4Address ipAddress): m_mask(mask), m_address(ipAddress) {}

    /**
     * \ingroup diffserv
     * \brief Match the packet's destination IP address using the stored mask.
     * \param pkt The packet to inspect.
     * \returns true if the masked destination IP matches, false otherwise.
     */
    bool
    DestinationMask::Match(Ptr<Packet> pkt) const
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

        // If we can't access the packet header, return false
        if (!packetCopy->RemoveHeader(ipv4Header))
        {
            NS_LOG_WARN("Packet does not contain IPv4 header");
            return false;
        }

        // Apply the mask and compare
        Ipv4Address destinationIpAddress = ipv4Header.GetDestination();

        // NS3 standard is to prefer CombineMask over IsMatch (which use the same underlying functionality)
        return (destinationIpAddress.CombineMask(m_mask) == m_address.CombineMask(m_mask));
    }
} // namespace ns3
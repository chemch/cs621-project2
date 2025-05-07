#include "source-mask.h"
#include "ns3/ipv4-header.h"
#include "source-mask.h"
#include "ns3/ppp-header.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("SourceMask");  

namespace ns3 {
    /**
     * \ingroup diffserv
     * \brief Constructor for SourceMask.
     * \param mask The source IP mask to apply.
     * \param ipAddress The source IP address to match.
     */
    SourceMask::SourceMask(Ipv4Mask mask, Ipv4Address ipAddress): m_mask(mask), m_address(ipAddress) {}

    /**
     * \ingroup diffserv
     * \brief Match the packet's source IP address using the stored mask.
     * \param pkt The packet to inspect.
     * \returns true if the masked source IP matches, false otherwise.
     */
    bool SourceMask::Match(Ptr<Packet> pkt) const
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

        // If we can't access the packet header, return false
        if (!packetCopy->RemoveHeader(ipv4Header))
        {
            NS_LOG_WARN("Packet does not contain IPv4 header");
            return false;
        }

        // Apply the mask and compare
        Ipv4Address sourceIpAddress = ipv4Header.GetSource();

        // NS3 standard is to prefer CombineMask over IsMatch (which do the same thing)
        return (sourceIpAddress.CombineMask(m_mask) == m_address.CombineMask(m_mask));
    }
} // namespace ns3
#include "ns3/ipv4-header.h"
#include "destination-mask.h"
#include "ns3/ppp-header.h"

namespace ns3 {

/**
 * \ingroup diffserv
 * \brief Constructor for DestinationMask.
 * \param mask The destination IP mask to apply.
 * \param ipAddress The destination IP address to match.
 */
DestinationMask::DestinationMask(Ipv4Mask mask, Ipv4Address ipAddress)
  : m_mask(mask), m_address(ipAddress) {}

/**
 * \ingroup diffserv
 * \brief Match the packet's destination IP address using the stored mask.
 * \param pkt The packet to inspect.
 * \returns true if the masked destination IP matches, false otherwise.
 */
bool
DestinationMask::Match(Ptr<Packet> pkt) const
{
    Ipv4Header ipv4Header;
    Ptr<Packet> packetCopy = pkt->Copy();

    PppHeader pppHeader;

    if (!packetCopy->RemoveHeader(pppHeader))
    {
        std::cout << "[PortMatch] Failed to remove PPP header" << std::endl;
        return false;
    }

    // If we can't access the packet header, return false
    if (!packetCopy->RemoveHeader(ipv4Header))
    {
        return false;
    }

    // Apply the mask and compare
    Ipv4Address destinationIpAddress = ipv4Header.GetDestination();

    // NS3 standard is to prefer CombineMask over IsMatch (which use the same underlying functionality)
    return (destinationIpAddress.CombineMask(m_mask) == m_address.CombineMask(m_mask));
}

} // namespace ns3
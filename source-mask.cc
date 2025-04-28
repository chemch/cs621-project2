#include "source-mask.h"
#include "ns3/ipv4-header.h"
#include "source-mask.h"

namespace ns3 {

/**
 * \ingroup diffserv
 * \brief Constructor for SourceMask.
 * \param mask The source IP mask to apply.
 * \param ipAddress The source IP address to match.
 */
SourceMask::SourceMask(Ipv4Mask mask, Ipv4Address ipAddress)
  : m_mask(mask), m_address(ipAddress) {}

/**
 * \ingroup diffserv
 * \brief Match the packet's source IP address using the stored mask.
 * \param pkt The packet to inspect.
 * \returns true if the masked source IP matches, false otherwise.
 */
bool
SourceMask::Match(Ptr<Packet> pkt) const
{
    Ipv4Header ipv4Header;
    Ptr<Packet> packetCopy = pkt->Copy();

    // If we can't access the packet header, return false
    if (!packetCopy->PeekHeader(ipv4Header))
    {
        return false;
    }

    // Apply the mask and compare
    Ipv4Address sourceIpAddress = ipv4Header.GetSource();

    // NS3 standard is to prefer CombineMask over IsMatch (which do the same thing)
    return (sourceIpAddress.CombineMask(m_mask) == m_address.CombineMask(m_mask));
}

} // namespace ns3
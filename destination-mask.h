#ifndef DESTINATION_MASK_H
#define DESTINATION_MASK_H

#include "ns3/packet.h"
#include "ns3/ipv4-address.h"
#include "filter-element.h"


namespace ns3 {

/**
 * \ingroup diffserv
 * \brief Class to match destination IP Address using a mask.
 */
class DestinationMask : public FilterElement
{

public:
    /**
     * \brief Constructor.
     * \param mask The destination IP mask to apply.
     * \param ipAddress The destination IP address to match.
     */
    DestinationMask(Ipv4Mask mask, Ipv4Address ipAddress);

    /**
     * \brief Match the packet's destination IP address using the provided mask.
     * \param pkt The packet to inspect.
     * \returns true if the masked destination IP matches, false otherwise.
     */
    bool Match(Ptr<Packet> pkt) const override;

private:
    Ipv4Mask m_mask;
    Ipv4Address m_address;
};

} // namespace ns3

#endif // DESTINATION_MASK_H
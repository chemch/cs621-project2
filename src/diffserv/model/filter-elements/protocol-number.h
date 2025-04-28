#ifndef PROTOCOL_NUMBER_H
#define PROTOCOL_NUMBER_H

#include "ns3/packet.h"
#include "ns3/object.h"
#include "filter-element.h"

namespace ns3 {

/**
 * \ingroup diffserv
 * \brief Class to determine if the specified protocol number is in the packet IP Header.
 */
class ProtocolNumber : public FilterElement
{

public:
    /**
     * \brief Constructor.
     * \param protocolNumber The protocol number to match (6 = TCP, 17 = UDP).
     */
    ProtocolNumber(uint8_t protocolNumber);

    /**
     * \brief Match the packet against the stored protocol number.
     * \param pkt The packet to check for the protocol number match.
     * \returns true if the protocol number is the same, false otherwise.
     */
    bool Match(Ptr<Packet> pkt) const override;

private:
    uint8_t m_protocolNumber;
};

} // namespace ns3

#endif // PROTOCOL_NUMBER_H
#ifndef SOURCE_PORT_NUMBER_H
#define SOURCE_PORT_NUMBER_H

#include "ns3/packet.h"
#include "ns3/object.h"
#include "filter-element.h"

namespace ns3 {

/**
 * \ingroup diffserv
 * \brief Class to match the source port number in a packet's L4 (Transport) header.
 */
class SourcePortNumber : public FilterElement
{

public:
    /**
     * \brief Constructor.
     * \param sourcePort The source port number to match.
     */
    SourcePortNumber(uint32_t sourcePort);

    /**
     * \brief Match the packet against the referenced source port number.
     * \param pkt The packet to inspect.
     * \returns true if the source port matches, false otherwise.
     */
    bool Match(Ptr<Packet> pkt) const override;

private:
    uint32_t m_sourcePort;
};

} // namespace ns3

#endif // SOURCE_PORT_NUMBER_H
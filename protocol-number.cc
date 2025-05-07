#include "ns3/ipv4-header.h"
#include "protocol-number.h"
#include "ns3/ppp-header.h"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("ProtocolNumber");  

namespace ns3 {
    /**
     * \ingroup diffserv
     * \brief Constructor for ProtocolNumber.
     * \param protocolNumber The protocol number to match.
     */
    ProtocolNumber::ProtocolNumber(uint8_t protocolNumber): m_protocolNumber(protocolNumber) {}

    /**
     * \ingroup diffserv
     * \brief Match the packet against the stored protocol number.
     * \param pkt The packet to inspect.
     * \returns true if the protocol number matches, false otherwise.
     */
    bool
    ProtocolNumber::Match(Ptr<Packet> pkt) const
    {
        // Make a copy of the packet first to simplify header access
        Ptr<Packet> packetCopy = pkt->Copy();

        // Use Headers to remove the PPP header and check the protocol number
        Ipv4Header ipv4Header;
        PppHeader pppHeader;

        if (!packetCopy->RemoveHeader(pppHeader))
        {
            NS_LOG_WARN("Packet does not contain PPP header");
            return false;
        }

        // If we can't peek at the IPv4 header, the packet doesn't match.
        if (!packetCopy->RemoveHeader(ipv4Header))
        {
            NS_LOG_WARN("Packet does not contain IPv4 header");
            return false;
        }

        // Compare protocol number from the header
        return ipv4Header.GetProtocol() == m_protocolNumber;
    }
} // namespace ns3
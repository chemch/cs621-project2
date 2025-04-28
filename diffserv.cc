// ns-3 core modules
#include "ns3/ipv4-header.h"
#include "ns3/packet.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "ns3/core-module.h"

// ns-3 core modules
#include "ns3/ipv4-header.h"
#include "ns3/packet.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"
#include "ns3/core-module.h"

#include "diffserv.h"


using namespace ns3;

// Standard C++ entry point for test programs
int main(int argc, char *argv[])
{
    std::cout << "Running SourceIPAddress Test..." << std::endl;

    Ptr<Packet> packet = Create<Packet>();

    Ipv4Header ipv4Header;
    ipv4Header.SetSource(Ipv4Address("192.168.1.100"));
    ipv4Header.SetDestination(Ipv4Address("192.168.1.200"));
    ipv4Header.SetProtocol(6); // TCP

    packet->AddHeader(ipv4Header);

    SourceIPAddress filter(Ipv4Address("192.168.1.100"));

    bool matched = filter.Match(packet);

    if (matched)
    {
        std::cout << "✅ Source IP matched!" << std::endl;
    }
    else
    {
        std::cout << "❌ Source IP did not match." << std::endl;
    }

    return 0;
}
// diffserv-tests.cc

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "destination-ip-address.h" // Your DestinationIPAddress class

using namespace ns3;

// Define your Test Class
class DiffservTests
{
public:
  DiffservTests() {}

  // Test method for Destination IP Address
  void TestDestinationIpAddressMatch()
  {
    std::cout << "Running TestDestinationIpAddressMatch..." << std::endl;

    // Create a Packet
    Ptr<Packet> packet = Create<Packet>(100);

    // Create and add an IPv4 header
    Ipv4Header ipv4Header;
    ipv4Header.SetDestination(Ipv4Address("10.1.2.3"));
    ipv4Header.SetSource(Ipv4Address("1.2.3.4"));
    ipv4Header.SetProtocol(17); // UDP
    ipv4Header.SetPayloadSize(100);

    packet->AddHeader(ipv4Header);

    // Create the DestinationIPAddress object
    DestinationIPAddress matcher(Ipv4Address("10.1.2.3"));

    // Perform Match
    bool isMatch = matcher.Match(packet);

    if (isMatch)
    {
      std::cout << "✅ Match successful: packet destination matches expected 10.1.2.3" << std::endl;
    }
    else
    {
      std::cout << "❌ Match failed: packet destination did not match" << std::endl;
    }

    // Negative test: wrong destination
    DestinationIPAddress wrongMatcher(Ipv4Address("192.168.1.1"));
    bool isWrongMatch = wrongMatcher.Match(packet);

    if (!isWrongMatch)
    {
      std::cout << "✅ Correctly failed to match wrong destination 192.168.1.1" << std::endl;
    }
    else
    {
      std::cout << "❌ Incorrectly matched wrong destination!" << std::endl;
    }
  }
};
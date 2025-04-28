// diffserv-tests.cc
#include "diffserv-tests.h"
#include "destination-ip-address.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"

using namespace ns3;

DiffservTests::DiffservTests() {}

void DiffservTests::RunAll()
{
  std::cout << "=== Running DiffservTests::RunAll() ===" << std::endl;

  int total = 0;
  int passed = 0;

  if (TestDestinationIPAddress()) passed++; total++;

  std::cout << "=== Tests complete: " << passed << "/" << total
            << " passed ===" << std::endl;
}

bool DiffservTests::TestDestinationIPAddress()
{
  std::cout << "[TestDestinationIPAddress]" << std::endl;

  Ptr<Packet> packet = Create<Packet>(100);

  Ipv4Header header;
  header.SetDestination(Ipv4Address("10.1.2.3"));
  packet->AddHeader(header);

  DestinationIPAddress matcher(Ipv4Address("10.1.2.3"));  // Correct class name

  if (!matcher.Match(packet))
  {
    std::cout << "❌ FAILED: Destination 10.1.2.3 did not match" << std::endl;
    return false;
  }
  else
  {
    std::cout << "✅ PASSED: Correct destination 10.1.2.3 matched" << std::endl;
  }

  // Also test a wrong match
  DestinationIPAddress wrongMatcher(Ipv4Address("192.168.1.1"));
  if (wrongMatcher.Match(packet))
  {
    std::cout << "❌ FAILED: Incorrect match with wrong destination 192.168.1.1" << std::endl;
    return false;
  }
  else
  {
    std::cout << "✅ PASSED: No match for wrong destination 192.168.1.1" << std::endl;
  }

  return true;
}
// diffserv-tests.cc
#include "diffserv-tests.h"
#include "destination-ip-address.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"

using namespace ns3;

DiffservTests::DiffservTests() {}

/**
 * \ingroup diffserv
 * \brief Run all Diffserv tests.
 *
 * This function runs all the tests defined in the DiffservTests class and
 * prints the results to the console.
 */
void DiffservTests::RunAll()
{
  // Keep track of the number of tests and passed tests
  int total = 0;
  int passed = 0;

  // Run each test and check the result
  if (TestDestinationIPAddress()) passed++; total++;

  // Print summary of the tests
  std::cout << "-- Tests complete: " << passed << "/" << total
            << " passed --" << std::endl;
}

/**
 * \ingroup diffserv
 * \brief Test the DestinationIPAddress filter element.
 *
 * This test creates a packet with a specific destination IP address and checks
 * if the DestinationIPAddress filter element correctly matches it.
 *
 * \returns true if the test passes, false if not.
 */
bool DiffservTests::TestDestinationIPAddress()
{
  std::cout << "-- [TestDestinationIPAddress] --" << std::endl;

  // Create a packet with a specific destination IP address
  Ptr<Packet> pkt = Create<Packet>(100);

  // Add an IPv4 header to the packet
  Ipv4Header header;
  header.SetDestination(Ipv4Address("8.8.8.8"));
  pkt->AddHeader(header);

  // Create a DestinationIPAddress reference
  DestinationIPAddress matchingFilterElement(Ipv4Address("8.8.8.8"));

  // Check if the packet matches the filter element (it should)
  if (!matchingFilterElement.Match(pkt))
  {
    std::cout << "\tFAILED: Destination did not match." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: Correct destination matched." << std::endl;
  }

  // Also test a wrong match 
  DestinationIPAddress wrongMatcher(Ipv4Address("4.4.4.4"));
  if (wrongMatcher.Match(pkt))
  {
    // Failure would indicate that the filter element matched a packet incorrectly
    std::cout << "\tFAILED: Incorrect match with wrong destination." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: No match for wrong destination." << std::endl;
  }

  return true;
}
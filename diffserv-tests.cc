#include "diffserv-tests.h"
#include "destination-ip-address.h"
#include "source-ip-address.h"
#include "source-port-number.h"
#include "destination-port-number.h"  // <<< Added if you have DestinationPortNumber class
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"

using namespace ns3;

DiffservTests::DiffservTests() {}

/**
 * \ingroup diffserv
 * \brief Run all Diffserv tests.
 */
void DiffservTests::RunAll()
{
  int total = 0;
  int passed = 0;

  if (TestDestinationIPAddress()) passed++; total++;
  if (TestSourceIPAddress()) passed++; total++;
  if (TestSourcePortNumber()) passed++; total++;
  if (TestDestinationPortNumber()) passed++; total++;

  std::cout << "-- Tests complete: " << passed << "/" << total
            << " passed --" << std::endl;
}

/**
 * \brief Test the DestinationIPAddress filter element.
 */
bool DiffservTests::TestDestinationIPAddress()
{
  std::cout << "-- [TestDestinationIPAddress] --" << std::endl;

  Ptr<Packet> pkt = Create<Packet>(10);
  Ipv4Header header;
  header.SetDestination(Ipv4Address("8.8.8.8"));
  pkt->AddHeader(header);

  DestinationIPAddress matchingFilterElement(Ipv4Address("8.8.8.8"));
  if (!matchingFilterElement.Match(pkt))
  {
    std::cout << "\tFAILED: Destination did not match." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: Correct destination matched." << std::endl;
  }

  DestinationIPAddress wrongMatcher(Ipv4Address("4.4.4.4"));
  if (wrongMatcher.Match(pkt))
  {
    std::cout << "\tFAILED: Incorrect match with wrong destination." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: No match for wrong destination." << std::endl;
  }

  return true;
}

/**
 * \brief Test the SourceIPAddress filter element.
 */
bool DiffservTests::TestSourceIPAddress()
{
  std::cout << "-- [TestSourceIPAddress] --" << std::endl;

  Ptr<Packet> pkt = Create<Packet>(10);
  Ipv4Header header;
  header.SetSource(Ipv4Address("1.1.1.1"));
  pkt->AddHeader(header);

  SourceIPAddress matchingFilterElement(Ipv4Address("1.1.1.1"));
  if (!matchingFilterElement.Match(pkt))
  {
    std::cout << "\tFAILED: Source did not match." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: Correct source matched." << std::endl;
  }

  SourceIPAddress wrongMatcher(Ipv4Address("5.5.5.5"));
  if (wrongMatcher.Match(pkt))
  {
    std::cout << "\tFAILED: Incorrect match with wrong source." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: No match for wrong source." << std::endl;
  }

  return true;
}

/**
 * \brief Test the SourcePortNumber filter element.
 */
bool DiffservTests::TestSourcePortNumber()
{
  std::cout << "-- [TestSourcePortNumber] --" << std::endl;

  // Test TCP packet
  {
    Ptr<Packet> pkt = Create<Packet>(10);

    TcpHeader tcpHeader;
    tcpHeader.SetSourcePort(7500);
    tcpHeader.SetDestinationPort(8080);

    Ipv4Header ipv4Header;
    ipv4Header.SetProtocol(6); // TCP

    pkt->AddHeader(tcpHeader);
    pkt->AddHeader(ipv4Header);

    SourcePortNumber matchingFilterElementTCP(7500);
    if (!matchingFilterElementTCP.Match(pkt))
    {
      std::cout << "\tFAILED: TCP source port did not match." << std::endl;
      return false;
    }
    else
    {
      std::cout << "\tPASSED: TCP source port matched." << std::endl;
    }

    SourcePortNumber nonMatchingFilterElementTCP(7000);
    if (nonMatchingFilterElementTCP.Match(pkt))
    {
      std::cout << "\tFAILED: Incorrect TCP source port matched." << std::endl;
      return false;
    }
    else
    {
      std::cout << "\tPASSED: TCP wrong source port did not match." << std::endl;
    }
  }

  // Test UDP packet
  {
    Ptr<Packet> pkt = Create<Packet>(10);

    UdpHeader udpHeader;
    udpHeader.SetSourcePort(33333);
    udpHeader.SetDestinationPort(23);

    Ipv4Header ipv4Header;
    ipv4Header.SetProtocol(17); // UDP

    pkt->AddHeader(udpHeader);
    pkt->AddHeader(ipv4Header);

    SourcePortNumber matchingFilterElementUDP(33333);
    if (!matchingFilterElementUDP.Match(pkt))
    {
      std::cout << "\tFAILED: UDP source port did not match." << std::endl;
      return false;
    }
    else
    {
      std::cout << "\tPASSED: UDP source port matched." << std::endl;
    }

    SourcePortNumber nonMatchingFilterElementUDP(22222);
    if (nonMatchingFilterElementUDP.Match(pkt))
    {
      std::cout << "\tFAILED: Incorrect UDP source port matched." << std::endl;
      return false;
    }
    else
    {
      std::cout << "\tPASSED: UDP wrong source port did not match." << std::endl;
    }
  }

  return true;
}

/**
 * \brief Test the DestinationPortNumber filter element.
 */
bool DiffservTests::TestDestinationPortNumber()
{
  std::cout << "-- [TestDestinationPortNumber] --" << std::endl;

  // Test TCP packet
  {
    Ptr<Packet> pkt = Create<Packet>(10);

    TcpHeader tcpHeader;
    tcpHeader.SetSourcePort(7500);
    tcpHeader.SetDestinationPort(8080);

    Ipv4Header ipv4Header;
    ipv4Header.SetProtocol(6); // TCP

    pkt->AddHeader(tcpHeader);
    pkt->AddHeader(ipv4Header);

    DestinationPortNumber matchingFilterElementTCP(8080);
    if (!matchingFilterElementTCP.Match(pkt))
    {
      std::cout << "\tFAILED: TCP destination port did not match." << std::endl;
      return false;
    }
    else
    {
      std::cout << "\tPASSED: TCP destination port matched." << std::endl;
    }

    DestinationPortNumber nonMatchingFilterElementTCP(7000);
    if (nonMatchingFilterElementTCP.Match(pkt))
    {
      std::cout << "\tFAILED: Incorrect TCP destination port matched." << std::endl;
      return false;
    }
    else
    {
      std::cout << "\tPASSED: TCP wrong destination port did not match." << std::endl;
    }
  }

  // Test UDP packet
  {
    Ptr<Packet> pkt = Create<Packet>(10);

    UdpHeader udpHeader;
    udpHeader.SetSourcePort(33333);
    udpHeader.SetDestinationPort(100);

    Ipv4Header ipv4Header;
    ipv4Header.SetProtocol(17); // UDP

    pkt->AddHeader(udpHeader);
    pkt->AddHeader(ipv4Header);

    DestinationPortNumber matchingFilterElementUDP(100);
    if (!matchingFilterElementUDP.Match(pkt))
    {
      std::cout << "\tFAILED: UDP destination port did not match." << std::endl;
      return false;
    }
    else
    {
      std::cout << "\tPASSED: UDP destination port matched." << std::endl;
    }

    DestinationPortNumber nonMatchingFilterElementUDP(22222);
    if (nonMatchingFilterElementUDP.Match(pkt))
    {
      std::cout << "\tFAILED: Incorrect UDP destination port matched." << std::endl;
      return false;
    }
    else
    {
      std::cout << "\tPASSED: UDP wrong destination port did not match." << std::endl;
    }
  }

  return true;
}
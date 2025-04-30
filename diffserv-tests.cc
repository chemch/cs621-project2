#include "diffserv-tests.h"
#include "destination-ip-address.h"
#include "source-ip-address.h"
#include "filter.h"
#include "diff-serv.h"
#include "source-port-number.h"
#include "destination-port-number.h" 
#include "protocol-number.h"
#include "source-mask.h"
#include "traffic-class.h"
#include "filter-element.h"
#include "destination-mask.h"
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
  if (TestSourceMask()) passed++; total++;
  if (TestDestinationMask()) passed++; total++;
  if (TestProtocolNumber()) passed++; total++;
  if (TestFilter()) passed++; total++;
  if (TestTrafficClass()) passed++; total++;
  if (TestDiffServ()) passed++; total++;

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

/**
 * \ingroup diffserv
 * \brief Test the DestinationMask filter element.
 *
 * This test creates packets with specific IP addresses and checks
 * if the DestinationMask filter element correctly matches using masking logic.
 *
 * \returns true if the test passes, false otherwise.
 */
bool DiffservTests::TestDestinationMask()
{
  std::cout << "-- [TestDestinationMask] --" << std::endl;

  Ptr<Packet> pkt = Create<Packet>(10);

  // Create an IPv4 header with IP
  Ipv4Header header;
  header.SetDestination(Ipv4Address("192.168.1.5"));
  pkt->AddHeader(header);

  // Mask: /24 subnet mask (255.0.0.0)
  Ipv4Mask mask("255.0.0.0");

  // Base network address that should match after masking
  Ipv4Address expectedNetwork("192.0.0.0");

  // Create Mask filter
  DestinationMask matchingFilterElement(mask, expectedNetwork);

  // Check match
  if (!matchingFilterElement.Match(pkt))
  {
    std::cout << "\tFAILED: Destination mask match failed." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: Mask matched correctly." << std::endl;
  }

  // Now test a non-matching case
  DestinationMask wrongFilterElement(mask, Ipv4Address("19.0.0.0"));
  if (wrongFilterElement.Match(pkt))
  {
    std::cout << "\tFAILED: Incorrect mask matched." << std::endl;
    return false;
  }
  else
  {
    // Correct mask would've been 19.0.0.0
    std::cout << "\tPASSED: Incorrect mask did not match (which is correct)." << std::endl;
  }

  return true;
}


/**
 * \ingroup diffserv
 * \brief Test the SourceMask filter element.
 *
 * This test creates packets with specific source IP addresses and checks
 * if the SourceMask filter element correctly matches using masking logic.
 *
 * \returns true if the test passes, false otherwise.
 */
bool DiffservTests::TestSourceMask()
{
  std::cout << "-- [TestSourceMask] --" << std::endl;

  Ptr<Packet> pkt = Create<Packet>(10);

  // Create an IPv4 header with source IP
  Ipv4Header header;
  header.SetSource(Ipv4Address("192.168.1.1"));
  pkt->AddHeader(header);

  // Mask: /24 subnet mask (255.255.0.0)
  Ipv4Mask mask("255.255.0.0");

  // Base network address that should match after masking
  Ipv4Address expectedNetwork("192.168.0.0");

  // Create SourceMask filter
  SourceMask matchingFilterElement(mask, expectedNetwork);

  // Check match
  if (!matchingFilterElement.Match(pkt))
  {
    std::cout << "\tFAILED: Source mask match failed." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: Source mask matched correctly." << std::endl;
  }

  // Now test a non-matching case
  SourceMask wrongFilterElement(mask, Ipv4Address("192.167.2.0"));
  if (wrongFilterElement.Match(pkt))
  {
    std::cout << "\tFAILED: Incorrect source mask matched." << std::endl;
    return false;
  }
  else
  {
    // Correct source mask would've been 192.167.0.0
    std::cout << "\tPASSED: Incorrect source mask did not match (which is correct)." << std::endl;
  }

  return true;
}

/**
 * \ingroup diffserv
 * \brief Test the ProtocolNumber filter element.
 *
 * This test creates packets with specific protocol numbers and checks
 * if the ProtocolNumber filter element correctly matches.
 *
 * \returns true if the test passes.
 */
bool DiffservTests::TestProtocolNumber()
{
  std::cout << "-- [TestProtocolNumber] --" << std::endl;

  Ptr<Packet> pkt = Create<Packet>(10);

  // Create an IPv4 header with protocol TCP
  Ipv4Header header;
  header.SetProtocol(6);
  pkt->AddHeader(header);

  // Create ProtocolNumber matcher for TCP
  ProtocolNumber matchingFilterElement(6);

  if (!matchingFilterElement.Match(pkt))
  {
    std::cout << "\tFAILED: Protocol TCP match failed." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: Protocol TCP matched correctly." << std::endl;
  }

  // Now create a matcher expecting UDP (17), which should NOT match
  ProtocolNumber wrongMatcher(17);
  if (wrongMatcher.Match(pkt))
  {
    std::cout << "\tFAILED: Incorrect protocol (UDP) matched." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: Incorrect protocol (UDP) did not match (correct)." << std::endl;
  }

  return true;
}

/**
 * \ingroup diffserv
 * \brief Test the Filter class.
 *
 * This test creates a Filter with dummy filter elements that either match or don't match,
 * and checks if the Filter::Match() behaves correctly.
 *
 * \returns true if the test passes.
 */
bool DiffservTests::TestFilter()
{
  std::cout << "-- [TestFilter] --" << std::endl;

  // Dummy class to simulate FilterElements
  class TestFilterElement : public FilterElement
  {
  public:

    // Constructor
    TestFilterElement(bool shouldMatch)
      : m_shouldMatch(shouldMatch) {}

    // Override the Match method (just return the stored value)
    bool Match(Ptr<Packet> packet) const override
    {
      return m_shouldMatch;
    }

  private:
    bool m_shouldMatch;
  };

  // Create a packet to test against
  Ptr<Packet> pkt = Create<Packet>(10);

  // Test case where all filter elements match
  Filter filterAllMatch;
  filterAllMatch.addFilterElement(new TestFilterElement(true));
  filterAllMatch.addFilterElement(new TestFilterElement(true));

  // Should match since all elements are set to true (directly)
  if (!filterAllMatch.Match(pkt))
  {
    std::cout << "\tFAILED: All filter elements should have matched." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: All filter elements matched correctly." << std::endl;
  }

  // Test case where one filter element fails
  Filter filterOneFails;
  filterOneFails.addFilterElement(new TestFilterElement(true));
  filterOneFails.addFilterElement(new TestFilterElement(false));

  // Should fail since one element is set to false
  if (filterOneFails.Match(pkt))
  {
    std::cout << "\tFAILED: Filter matched even though one element should have failed." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: Filter correctly failed when one element did not match." << std::endl;
  }

  return true;
}

/**
 * \ingroup diffserv
 * \brief Test the TrafficClass.
 *
 * This test checks queue operations, filter matching, and initial state behavior.
 *
 * \returns true if the test passes. Return false on unexpected fails. 
 */
bool DiffservTests::TestTrafficClass()
{
  std::cout << "-- [TestTrafficClass] --" << std::endl;

  TrafficClass trafficClass;

  // Should be empty initially
  if (!trafficClass.IsEmpty())
  {
    std::cout << "\tFAILED: TrafficClass should be empty initially." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: TrafficClass initially empty." << std::endl;
  }

  // Check default max packets
  if (trafficClass.GetMaxPackets() != 100)
  {
    std::cout << "\tFAILED: Default max packets should be 100." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: Default max packets is 100." << std::endl;
  }

  // Enqueue a packet
  Ptr<Packet> pkt = Create<Packet>(20);
  if (!trafficClass.Enqueue(pkt))
  {
    std::cout << "\tFAILED: Enqueue failed unexpectedly." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: Packet enqueued successfully." << std::endl;
  }

  if (trafficClass.IsEmpty())
  {
    std::cout << "\tFAILED: TrafficClass should not be empty after Enqueue." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: TrafficClass is not empty after Enqueue." << std::endl;
  }

  // Peek packet
  Ptr<Packet> peekPkt = trafficClass.Peek();
  if (peekPkt == nullptr)
  {
    std::cout << "\tFAILED: Peek returned null packet." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: Peek returned a valid packet." << std::endl;
  }

  // Dequeue packet
  Ptr<Packet> dequeuedPkt = trafficClass.Dequeue();
  if (dequeuedPkt == nullptr)
  {
    std::cout << "\tFAILED: Dequeue returned null packet." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: Dequeue returned a valid packet." << std::endl;
  }

  if (!trafficClass.IsEmpty())
  {
    std::cout << "\tFAILED: TrafficClass should be empty after Dequeue." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: TrafficClass empty after Dequeue." << std::endl;
  }

  // Test Filter class - always true (match)
  class TestFilter : public Filter
  {
  public:
    bool Match(Ptr<Packet> pkt) const
    {
      return true;
    }
  };

  // Important Test Here (Test Filter and Match Functionality)
  TestFilter* testFilter = new TestFilter();
  trafficClass.AddFilter(testFilter);

  // Check that there is a match on the packet
  if (!trafficClass.Match(pkt))
  {
    std::cout << "\tFAILED: TrafficClass Match() failed with Filter." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: TrafficClass Match() worked perfectly. Yes." << std::endl;
  }

  return true;
}

/**
 * \ingroup diffserv
 * \brief Test the DiffServ class enqueue, dequeue, and classify behavior.
 */
bool DiffservTests::TestDiffServ()
{
  std::cout << "-- [TestDiffServ] --" << std::endl;

  class DummyTrafficClass : public TrafficClass
  {
  public:
    bool Match(Ptr<Packet> pkt) const
    {
      return true;
    }

    bool Enqueue(Ptr<Packet> pkt)
    {
      m_pkt = pkt;
      return true;
    }

    Ptr<Packet> Dequeue()
    {
      return m_pkt;
    }

    Ptr<Packet> Peek() const
    {
      return m_pkt;
    }

    Ptr<Packet> Remove()
    {
      return m_pkt;
    }

  private:
    Ptr<Packet> m_pkt;
  };







  // Create DiffServ instance and test traffic class
  class TestDiffServImpl : public DiffServ
  {
  public:

    /**
     * Define test Schedule behavior to always return a packet.
     * This is a dummy implementation for testing purposes.
     *  */  
    Ptr<const Packet> Schedule() const override
    {
      // Always return a scheduled packet for test
      if (!m_dummyPkt)
      {
        m_dummyPkt = Create<Packet>(30);
      }

      return m_dummyPkt;
    }

  private:
    mutable Ptr<Packet> m_dummyPkt;
  };

  Ptr<Packet> testPkt = Create<Packet>(42);

  TestDiffServImpl diffserv;
  DummyTrafficClass* trafficClass = new DummyTrafficClass();
  diffserv.AddQueue(trafficClass);

  // Test Enqueue
  if (!diffserv.Enqueue(testPkt))
  {
    std::cout << "\tFAILED: DiffServ failed to enqueue packet." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: DiffServ enqueue successful." << std::endl;
  }

  // Test Dequeue
  Ptr<Packet> dqPkt = diffserv.Dequeue();
  if (dqPkt == nullptr)
  {
    std::cout << "\tFAILED: DiffServ dequeue returned null." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: DiffServ dequeue returned valid packet." << std::endl;
  }

  // Test Peek
  Ptr<const Packet> peekPkt = diffserv.Peek();
  if (peekPkt == nullptr)
  {
    std::cout << "\tFAILED: DiffServ peek returned null." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: DiffServ peek returned valid packet." << std::endl;
  }

  // Test Remove (need to enqueue again)
  diffserv.Enqueue(testPkt);
  Ptr<Packet> rmPkt = diffserv.Remove();
  if (rmPkt == nullptr)
  {
    std::cout << "\tFAILED: DiffServ remove returned null." << std::endl;
    return false;
  }
  else
  {
    std::cout << "\tPASSED: DiffServ remove returned valid packet." << std::endl;
  }

  return true;
}
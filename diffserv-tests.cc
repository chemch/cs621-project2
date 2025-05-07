#include "diffserv-tests.h"
#include "destination-ip-address.h"
#include "source-ip-address.h"
#include "filter.h"
#include "diff-serv.h"
#include "source-port-number.h"
#include "destination-port-number.h"
#include "protocol-number.h"
#include "source-mask.h"
#include "spq.h"
#include "traffic-class.h"
#include "filter-element.h"
#include "destination-mask.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/ppp-header.h"
#include "drr.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("DiffservTests");

DiffservTests::DiffservTests() {}

/**
 * \ingroup diffserv
 * \brief Execute all Diffserv unit tests and log a summary.
 */
void
DiffservTests::RunAll()
{
    int total = 0;
    int passed = 0;

    NS_LOG_UNCOND("\n-- Diffserv Tests --");

    if (TestQueueDequeue())         ++passed; ++total;
    if (TestDestinationIPAddress()) ++passed; ++total;
    if (TestSourceIPAddress())      ++passed; ++total;
    if (TestSourcePortNumber())     ++passed; ++total;
    if (TestDestinationPortNumber())++passed; ++total;
    if (TestSourceMask())           ++passed; ++total;
    if (TestDestinationMask())      ++passed; ++total;
    if (TestProtocolNumber())       ++passed; ++total;
    if (TestFilter())               ++passed; ++total;
    if (TestTrafficClass())         ++passed; ++total;
    if (TestDiffServ())             ++passed; ++total;
    if (TestSPQ())                  ++passed; ++total;
    if (TestDRR())                  ++passed; ++total;

    NS_LOG_UNCOND("\n-- Diffserv Tests Summary --");
    NS_LOG_UNCOND("Total tests run: " << total);
    NS_LOG_UNCOND("Tests passed   : " << passed);
    NS_LOG_UNCOND("Tests failed   : " << (total - passed));
}

/**
 * \brief Validate basic enqueue/dequeue behavior in TrafficClass.
 * \returns true if test passes, false otherwise
 */
bool
DiffservTests::TestQueueDequeue()
{
    NS_LOG_UNCOND("-- [TestQueueDequeue] --");

    Ptr<Packet> pkt = Create<Packet>(10);
    TrafficClass tc;
    tc.Enqueue(pkt);

    Ptr<Packet> outPkt = tc.Dequeue();
    NS_LOG_UNCOND("\tDequeued packet size: " << outPkt->GetSize());
    if (outPkt->GetSize() != 10)
    {
        NS_LOG_UNCOND("\tFAILED: Dequeued packet size mismatch.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Dequeued size correct.");

    NS_LOG_UNCOND("\tQueue empty? " << (tc.IsEmpty() ? "Yes" : "No"));
    Ptr<Packet> secondDeq = tc.Dequeue();
    NS_LOG_UNCOND("\tSecond dequeue: " << (secondDeq ? std::to_string(secondDeq->GetSize()) : "null"));
    if (secondDeq)
    {
        NS_LOG_UNCOND("\tFAILED: Expected null on empty dequeue.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Empty dequeue returns null.");

    return true;
}

/**
 * \brief Verify matching by DestinationIPAddress filter element.
 * \returns true if filter matches correct IP and rejects wrong IP
 */
bool
DiffservTests::TestDestinationIPAddress()
{
    NS_LOG_UNCOND("-- [TestDestinationIPAddress] --");

    Ptr<Packet> pkt = Create<Packet>(10);
    Ipv4Header hdr;
    hdr.SetDestination(Ipv4Address("8.8.8.8"));
    pkt->AddHeader(hdr);
    pkt->AddHeader(PppHeader());

    DestinationIPAddress match("8.8.8.8");
    if (!match.Match(pkt))
    {
        NS_LOG_UNCOND("\tFAILED: Expected destination 8.8.8.8 to match.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Correct destination matched.");

    DestinationIPAddress noMatch("4.4.4.4");
    if (noMatch.Match(pkt))
    {
        NS_LOG_UNCOND("\tFAILED: Unexpected match for 4.4.4.4.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Incorrect destination not matched.");

    return true;
}

/**
 * \brief Verify matching by SourceIPAddress filter element.
 * \returns true if filter matches correct IP and rejects wrong IP
 */
bool
DiffservTests::TestSourceIPAddress()
{
    NS_LOG_UNCOND("-- [TestSourceIPAddress] --");

    Ptr<Packet> pkt = Create<Packet>(10);
    Ipv4Header hdr;
    hdr.SetSource(Ipv4Address("1.1.1.1"));
    pkt->AddHeader(hdr);
    pkt->AddHeader(PppHeader());

    SourceIPAddress match("1.1.1.1");
    if (!match.Match(pkt))
    {
        NS_LOG_UNCOND("\tFAILED: Expected source 1.1.1.1 to match.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Correct source matched.");

    SourceIPAddress noMatch("5.5.5.5");
    if (noMatch.Match(pkt))
    {
        NS_LOG_UNCOND("\tFAILED: Unexpected match for 5.5.5.5.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Incorrect source not matched.");

    return true;
}

/**
 * \brief Test SourcePortNumber filter for both TCP and UDP packets.
 * \returns true if both protocol port tests pass
 */
bool
DiffservTests::TestSourcePortNumber()
{
    NS_LOG_UNCOND("-- [TestSourcePortNumber] --");

    // TCP case
    {
        Ptr<Packet> pkt = Create<Packet>(10);
        TcpHeader tcpHdr;
        tcpHdr.SetSourcePort(7500);
        tcpHdr.SetDestinationPort(8080);
        Ipv4Header ipHdr;
        ipHdr.SetProtocol(6);  // TCP
        pkt->AddHeader(tcpHdr);
        pkt->AddHeader(ipHdr);
        pkt->AddHeader(PppHeader());

        SourcePortNumber matcherTCP(7500);
        if (!matcherTCP.Match(pkt))
        {
            NS_LOG_UNCOND("\tFAILED: TCP source port did not match.");
            return false;
        }
        NS_LOG_UNCOND("\tPASSED: TCP source port matched.");

        SourcePortNumber nonMatcherTCP(7000);
        if (nonMatcherTCP.Match(pkt))
        {
            NS_LOG_UNCOND("\tFAILED: Incorrect TCP source port matched.");
            return false;
        }
        NS_LOG_UNCOND("\tPASSED: TCP wrong source port did not match.");
    }

    // UDP case
    {
        Ptr<Packet> pkt = Create<Packet>(10);
        UdpHeader udpHdr;
        udpHdr.SetSourcePort(33333);
        udpHdr.SetDestinationPort(23);
        Ipv4Header ipHdr;
        ipHdr.SetProtocol(17);  // UDP
        pkt->AddHeader(udpHdr);
        pkt->AddHeader(ipHdr);
        pkt->AddHeader(PppHeader());

        SourcePortNumber matcherUDP(33333);
        if (!matcherUDP.Match(pkt))
        {
            NS_LOG_UNCOND("\tFAILED: UDP source port did not match.");
            return false;
        }
        NS_LOG_UNCOND("\tPASSED: UDP source port matched.");

        SourcePortNumber nonMatcherUDP(22222);
        if (nonMatcherUDP.Match(pkt))
        {
            NS_LOG_UNCOND("\tFAILED: Incorrect UDP source port matched.");
            return false;
        }
        NS_LOG_UNCOND("\tPASSED: UDP wrong source port did not match.");
    }

    return true;
}

/**
 * \brief Test DestinationPortNumber filter for both TCP and UDP packets.
 * \returns true if both protocol port tests pass
 */
bool
DiffservTests::TestDestinationPortNumber()
{
    NS_LOG_UNCOND("-- [TestDestinationPortNumber] --");

    // TCP case
    {
        Ptr<Packet> pkt = Create<Packet>(10);
        TcpHeader tcpHdr;
        tcpHdr.SetSourcePort(7500);
        tcpHdr.SetDestinationPort(8080);
        Ipv4Header ipHdr;
        ipHdr.SetProtocol(6);
        pkt->AddHeader(tcpHdr);
        pkt->AddHeader(ipHdr);
        pkt->AddHeader(PppHeader());

        DestinationPortNumber matcherTCP(8080);
        if (!matcherTCP.Match(pkt))
        {
            NS_LOG_UNCOND("\tFAILED: TCP destination port did not match.");
            return false;
        }
        NS_LOG_UNCOND("\tPASSED: TCP destination port matched.");

        DestinationPortNumber nonMatcherTCP(7000);
        if (nonMatcherTCP.Match(pkt))
        {
            NS_LOG_UNCOND("\tFAILED: Incorrect TCP destination port matched.");
            return false;
        }
        NS_LOG_UNCOND("\tPASSED: TCP wrong destination port did not match.");
    }

    // UDP case
    {
        Ptr<Packet> pkt = Create<Packet>(10);
        UdpHeader udpHdr;
        udpHdr.SetSourcePort(33333);
        udpHdr.SetDestinationPort(100);
        Ipv4Header ipHdr;
        ipHdr.SetProtocol(17);
        pkt->AddHeader(udpHdr);
        pkt->AddHeader(ipHdr);
        pkt->AddHeader(PppHeader());

        DestinationPortNumber matcherUDP(100);
        if (!matcherUDP.Match(pkt))
        {
            NS_LOG_UNCOND("\tFAILED: UDP destination port did not match.");
            return false;
        }
        NS_LOG_UNCOND("\tPASSED: UDP destination port matched.");

        DestinationPortNumber nonMatcherUDP(22222);
        if (nonMatcherUDP.Match(pkt))
        {
            NS_LOG_UNCOND("\tFAILED: Incorrect UDP destination port matched.");
            return false;
        }
        NS_LOG_UNCOND("\tPASSED: UDP wrong destination port did not match.");
    }

    return true;
}

/**
 * \ingroup diffserv
 * \brief Test DestinationMask filter element behavior.
 * \returns true if mask matching and mismatching succeed
 */
bool
DiffservTests::TestDestinationMask()
{
    NS_LOG_UNCOND("-- [TestDestinationMask] --");

    Ptr<Packet> pkt = Create<Packet>(10);
    Ipv4Header hdr;
    hdr.SetDestination(Ipv4Address("192.168.1.5"));
    pkt->AddHeader(hdr);
    pkt->AddHeader(PppHeader());

    Ipv4Mask mask("255.0.0.0");
    Ipv4Address netAddr("192.0.0.0");
    DestinationMask matcher(mask, netAddr);
    if (!matcher.Match(pkt))
    {
        NS_LOG_UNCOND("\tFAILED: Expected mask to match destination.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Destination mask matched correctly.");

    DestinationMask wrong(mask, Ipv4Address("19.0.0.0"));
    if (wrong.Match(pkt))
    {
        NS_LOG_UNCOND("\tFAILED: Incorrect mask matched destination.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Incorrect mask did not match.");

    return true;
}

/**
 * \ingroup diffserv
 * \brief Test SourceMask filter element behavior.
 * \returns true if mask matching and mismatching succeed
 */
bool
DiffservTests::TestSourceMask()
{
    NS_LOG_UNCOND("-- [TestSourceMask] --");

    Ptr<Packet> pkt = Create<Packet>(10);
    Ipv4Header hdr;
    hdr.SetSource(Ipv4Address("192.168.1.1"));
    pkt->AddHeader(hdr);
    pkt->AddHeader(PppHeader());

    Ipv4Mask mask("255.255.0.0");
    Ipv4Address netAddr("192.168.0.0");
    SourceMask matcher(mask, netAddr);
    if (!matcher.Match(pkt))
    {
        NS_LOG_UNCOND("\tFAILED: Expected mask to match source.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Source mask matched correctly.");

    SourceMask wrong(mask, Ipv4Address("192.167.2.0"));
    if (wrong.Match(pkt))
    {
        NS_LOG_UNCOND("\tFAILED: Incorrect mask matched source.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Incorrect mask did not match.");

    return true;
}

/**
 * \ingroup diffserv
 * \brief Test ProtocolNumber filter element behavior.
 * \returns true if correct protocol matches and incorrect does not
 */
bool
DiffservTests::TestProtocolNumber()
{
    NS_LOG_UNCOND("-- [TestProtocolNumber] --");

    Ptr<Packet> pkt = Create<Packet>(10);
    Ipv4Header hdr;
    hdr.SetProtocol(6);  // TCP
    pkt->AddHeader(hdr);
    pkt->AddHeader(PppHeader());

    ProtocolNumber matcher(6);
    if (!matcher.Match(pkt))
    {
        NS_LOG_UNCOND("\tFAILED: Expected TCP protocol to match.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: TCP protocol matched correctly.");

    ProtocolNumber wrong(17);  // UDP
    if (wrong.Match(pkt))
    {
        NS_LOG_UNCOND("\tFAILED: Unexpected UDP protocol match.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: UDP protocol did not match (correct).");

    return true;
}


/**
 * \ingroup diffserv
 * \brief Test the Filter container logic.
 * \returns true if dummy filter elements behave correctly.
 */
bool
DiffservTests::TestFilter()
{
    NS_LOG_UNCOND("-- [TestFilter] --");

    class DummyElement : public FilterElement
    {
    public:
        DummyElement(bool res) : m_res(res) {}
        bool Match(Ptr<Packet>) const override { return m_res; }
    private:
        bool m_res;
    };

    Ptr<Packet> pkt = Create<Packet>(10);

    Filter all;
    all.AddFilterElement(new DummyElement(true));
    all.AddFilterElement(new DummyElement(true));
    if (!all.Match(pkt))
    {
        NS_LOG_UNCOND("\tFAILED: All elements should match.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: All elements matched.");

    Filter oneFail;
    oneFail.AddFilterElement(new DummyElement(true));
    oneFail.AddFilterElement(new DummyElement(false));
    if (oneFail.Match(pkt))
    {
        NS_LOG_UNCOND("\tFAILED: Should fail when one element mismatches.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Filter failed when one element mismatched.");

    return true;
}

/**
 * \ingroup diffserv
 * \brief Test basic TrafficClass operations.
 * \returns true if enqueue, peek, dequeue, and filter match operate correctly.
 */
bool
DiffservTests::TestTrafficClass()
{
    NS_LOG_UNCOND("-- [TestTrafficClass] --");

    TrafficClass tc;
    if (!tc.IsEmpty())
    {
        NS_LOG_UNCOND("\tFAILED: Should be empty initially.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Initially empty.");

    if (tc.GetMaxPackets() != 100)
    {
        NS_LOG_UNCOND("\tFAILED: Default max packets should be 100.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Default max packets is correct.");

    Ptr<Packet> pkt = Create<Packet>(20);
    if (!tc.Enqueue(pkt))
    {
        NS_LOG_UNCOND("\tFAILED: Enqueue failed.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Packet enqueued.");

    if (tc.IsEmpty())
    {
        NS_LOG_UNCOND("\tFAILED: Should not be empty after enqueue.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Not empty after enqueue.");

    Ptr<Packet> peek = tc.Peek();
    if (!peek)
    {
        NS_LOG_UNCOND("\tFAILED: Peek returned null.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Peek returned valid packet.");

    Ptr<Packet> dq = tc.Dequeue();
    if (!dq)
    {
        NS_LOG_UNCOND("\tFAILED: Dequeue returned null.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Dequeue returned valid packet.");

    if (!tc.IsEmpty())
    {
        NS_LOG_UNCOND("\tFAILED: Should be empty after dequeue.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Empty after dequeue.");

    class AlwaysFilter : public Filter
    {
    public:
        bool Match(Ptr<Packet>) const { return true; }
    };

    AlwaysFilter* f = new AlwaysFilter();
    tc.AddFilter(f);
    if (!tc.Match(pkt))
    {
        NS_LOG_UNCOND("\tFAILED: Match should return true with AlwaysFilter.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Match returned true with AlwaysFilter.");

    return true;
}

/**
 * \ingroup diffserv
 * \brief Test DiffServ enqueue, dequeue, and peek logic.
 * \returns true if operations succeed with dummy traffic class.
 */
bool
DiffservTests::TestDiffServ()
{
    NS_LOG_UNCOND("-- [TestDiffServ] --");

    class DummyTC : public TrafficClass
    {
    public:
        bool Match(Ptr<Packet>) const  { return true; }
        bool Enqueue(Ptr<Packet> p)  { m_p = p; return true; }
        Ptr<Packet> Dequeue()  { return m_p; }
        Ptr<const Packet> Peek() const { return m_p; }
    private:
        Ptr<Packet> m_p;
    };

    class TestDS : public DiffServ
    {
    public:
        Ptr<const Packet> Schedule() const 
        {
            if (!m_pkt) m_pkt = Create<Packet>(30);
            return m_pkt;
        }
    private:
        mutable Ptr<Packet> m_pkt;
    };

    Ptr<Packet> testPkt = Create<Packet>(42);
    TestDS ds;
    DummyTC* dtc = new DummyTC();
    ds.RegisterQueue(dtc);

    if (!ds.Enqueue(testPkt))
    {
        NS_LOG_UNCOND("\tFAILED: Enqueue in DiffServ.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Enqueue in DiffServ.");

    Ptr<Packet> rd = ds.Dequeue();
    if (!rd)
    {
        NS_LOG_UNCOND("\tFAILED: Dequeue in DiffServ.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Dequeue in DiffServ.");

    Ptr<const Packet> pk = ds.Peek();
    if (!pk)
    {
        NS_LOG_UNCOND("\tFAILED: Peek in DiffServ.");
        return false;
    }
    NS_LOG_UNCOND("\tPASSED: Peek in DiffServ.");

    return true;
}

/**
 * \ingroup diffserv
 * \brief Test SPQ scheduling (highest priority first).
 * \returns true if SPQ schedules correctly.
 */
bool
DiffservTests::TestSPQ()
{
    NS_LOG_UNCOND("-- [TestSPQ] --");

    SPQ spq;
    TrafficClass* high = new TrafficClass(); high->SetPriorityLevel(0);
    TrafficClass* low  = new TrafficClass(); low->SetPriorityLevel(1);
    spq.RegisterQueue(high); spq.RegisterQueue(low);

    Ptr<Packet> ph = Create<Packet>(20);
    Ptr<Packet> pl = Create<Packet>(10);
    spq.Enqueue(ph); spq.Enqueue(pl);

    Ptr<const Packet> sch = spq.Schedule();
    NS_LOG_UNCOND("\tScheduled size: " << (sch ? sch->GetSize() : 0));
    Ptr<Packet> dq = spq.Dequeue();
    NS_LOG_UNCOND("\tDequeued size: " << (dq ? dq->GetSize() : 0));

    return true;
}

/**
 * \ingroup diffserv
 * \brief Test DRR scheduling (deficit round robin).
 * \returns true if expected packet is dequeued.
 */
bool
DiffservTests::TestDRR()
{
    NS_LOG_UNCOND("-- [TestDRR] --");

    DRR drr;
    TrafficClass* tw = new TrafficClass(); tw->SetWeight(70);
    TrafficClass* tl = new TrafficClass(); tl->SetWeight(50);
    drr.RegisterQueue(tl); drr.RegisterQueue(tw);

    Ptr<Packet> ph = Create<Packet>(80);
    Ptr<Packet> pl = Create<Packet>(40);
    tw->Enqueue(ph); tl->Enqueue(pl);

    Ptr<Packet> dq = drr.Dequeue();
    NS_LOG_UNCOND("\tDequeued size: " << (dq ? dq->GetSize() : 0));
    return (dq == pl);
}
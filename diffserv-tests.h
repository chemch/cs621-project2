// diffserv-tests.h
#ifndef DIFFSERV_TESTS_H
#define DIFFSERV_TESTS_H

#include "ns3/ipv4-address.h"
#include "ns3/packet.h"

namespace ns3 {

class DiffservTests
{
public:
  DiffservTests();

  // Run all the tests
  void RunAll();

private:
  // Individual Tests
  bool TestDestinationIPAddress();
  bool TestSourceIPAddress();
  bool TestSourcePortNumber();
  bool TestDestinationPortNumber();
  bool TestSourceMask();
  bool TestDestinationMask();
  bool TestProtocolNumber();
  bool TestFilter();
  bool TestTrafficClass();
};

} // namespace ns3

#endif // DIFFSERV_TESTS_H
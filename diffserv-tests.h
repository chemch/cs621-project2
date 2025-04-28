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

  // Runs all the tests
  void RunAll();

private:
  // Individual test cases
  bool TestDestinationIPAddress();
};

} // namespace ns3

#endif // DIFFSERV_TESTS_H
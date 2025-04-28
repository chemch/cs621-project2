// main.cc

#include "ns3/core-module.h"
#include "destination-ip-address-test.cc"

using namespace ns3;

int main(int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse(argc, argv);

  std::cout << "Starting DiffServ main program..." << std::endl;

  // Create a test runner object
  DiffservTests tests;

  // Call your test methods
  tests.TestDestinationIpAddressMatch();

  return 0;
}
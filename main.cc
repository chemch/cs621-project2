// main.cc
#include "ns3/core-module.h"
#include "diffserv-tests.h"

using namespace ns3;

int main(int argc, char *argv[])
{
  bool runTests = false;
  CommandLine cmd;
  cmd.AddValue("runTests", "Run unit tests (true/false)", runTests);
  cmd.Parse(argc, argv);


  // Allow running unit tests from the command line
  if (runTests)
  {
    std::cout << "Running Diffserv unit tests..." << std::endl;
    DiffservTests tests;
    tests.RunAll();  // 👈 manually call RunAll()
    return 0;
  }
  else
  {
    std::cout << "Running Diffserv simulation..." << std::endl;

    // Your real Diffserv simulation here

    return 0;
  }
}
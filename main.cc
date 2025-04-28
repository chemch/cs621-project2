// main.cc
#include "ns3/core-module.h"
#include "diffserv-tests.h"

using namespace ns3;

/**
 * \ingroup diffserv
 * \brief Main function for the Diffserv simulation.
 *
 * This function sets up the command line arguments and runs the Diffserv simulation.
 *
 * \param argc The number of command line arguments.
 * \param argv The command line arguments.
 * \returns 0 on success, non-zero on failure.
 */
int main(int argc, char *argv[])
{
  // Default to not run tests
  bool runTests = false;
  
  // Parse the command line arguments
  CommandLine cmd;
  cmd.AddValue("runTests", "Run unit tests (true/false)", runTests);
  cmd.Parse(argc, argv);

  // Run unit tests if specified via command line
  if (runTests)
  {
    std::cout << "\n- Running Unit Tests -" << std::endl;

    // Create an instance of DiffservTests
    // and run all tests
    DiffservTests tests;
    tests.RunAll();

    return 0;
  }
  else
  {
    std::cout << "\n- Running Simulation -" << std::endl;

    return 0;
  }
}
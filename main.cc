#include "diffserv-tests.h"
#include "simulation.h"
#include <iostream>
#include <string>
#include "json.hpp"
#include "ns3/simulator.h"
#include "ns3/core-module.h"
#include "ns3/command-line.h"
#include "main.h"

using namespace ns3;

// Include necessary headers for JSON parsing
using json = nlohmann::json;

// Define constants for the simulation
static constexpr double SIM_DURATION = 30.0;

/**
 * \brief Run the simulation based on the provided configuration file.
 * \param configFile The path to the configuration file.
 */
void ns3::RunSimulation(const std::string& configFile)
{
    // Initialize the simulation
    Simulation simulation;

    // Parse the configuration file
    if (simulation.parseConfigs(configFile))
    {
        NS_LOG_UNCOND("Failed to parse the configuration file.");
        return;
    }
    // Print the parsed configuration
    else
    {
        NS_LOG_UNCOND("Parsed configuration file successfully.");
        simulation.PrintConfig();
    }

    NS_LOG_UNCOND("Building base topology");
    simulation.InitializeTopology();

    NS_LOG_UNCOND("Building QoS mechanism");
    simulation.InitializeQosScheduler();

    NS_LOG_UNCOND("Initialize UDP Application");
    simulation.InitializeUdpApplication();

    NS_LOG_UNCOND("Starting simulation...");

    // Set the max simulation time
    Simulator::Stop(Seconds(SIM_DURATION));

    // Kick off the simulation
    Simulator::Run();
    Simulator::Destroy();

    NS_LOG_UNCOND("Simulation finished");
}

/**
 * \brief Main function to run the Diffserv simulation or tests.
 * \param argc The number of command line arguments.
 * \param argv The command line arguments.
 * \returns 0 on success, 1 on failure.
 */
int main(int argc, char* argv[])
{
    // Set logging levels for various components
    LogComponentEnable ("DestinationIPAddress", LOG_LEVEL_DEBUG);
    LogComponentEnable ("DestinationMask", LOG_LEVEL_DEBUG);
    LogComponentEnable ("DestinationPortNumber", LOG_LEVEL_DEBUG);
    LogComponentEnable ("ProtocolNumber", LOG_LEVEL_INFO);
    LogComponentEnable ("Filter", LOG_LEVEL_WARN);
    LogComponentEnable ("SourceMask", LOG_LEVEL_WARN);
    LogComponentEnable ("TrafficClass", LOG_LEVEL_WARN);

    // Set up the command line argument variables
    std::string runMode;
    std::string configFile;

    // Add command line arguments for run mode and config file
    ns3::CommandLine cmd;
    cmd.AddValue ("runMode",    "Mode: \"test\" or \"sim\"", runMode);
    cmd.AddValue ("configFile", "QoS JSON config (required if runMode==sim)", configFile);
    cmd.Parse (argc, argv);

    // Check if the run mode is set to "test" or "sim"
    if (runMode == "test")
    {
        // Run the Diffserv tests
        DiffservTests diffServTests;
        diffServTests.RunAll();
    }
    // Check if the run mode is simulation
    else if (runMode == "sim")
    {
        // Check if the config file is provided
        if (configFile.empty ())
        {
            NS_LOG_UNCOND("Error: --configFile must be set in sim mode");
            return 1;
        }

        RunSimulation (configFile);
    }
    // Otherwise, print an error message
    else
    {
        NS_LOG_UNCOND("Error: --runMode must be \"test\" or \"sim\"");
        return 1;
    }

    // Return 0 to indicate successful execution
    NS_LOG_UNCOND("Ending Application");
    return 0;
}
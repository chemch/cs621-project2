#include "diffserv-tests.h"
#include "simulation.h"
#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"
#include "ns3/simulator.h"
#include "ns3/core-module.h"
#include "ns3/command-line.h"

using namespace ns3;

// Include necessary headers for JSON parsing
using json = nlohmann::json;

/**
 * \brief Run the simulation based on the provided configuration file.
 * \param configFile The path to the configuration file.
 */
void RunSimulation(const std::string& configFile)
{
    // Initialize the simulation
    Simulation simulation;

    // Parse the configuration file
    if (simulation.parseConfigs(configFile))
    {
        std::cerr << "Failed to parse QoS config." << std::endl;
        return;
    }

    // Print the parsed configuration
    else
    {
        std::cout << "-- Parsed QoS Config --" << std::endl;
        simulation.PrintConfig();
    }

    std::cout << "-- Building Base Topology --" << std::endl;
    simulation.InitializeTopology();

    std::cout << "-- Build QoS Mechanism on UDP App --" << std::endl;
    simulation.InitializeQosScheduler();

    std::cout << "-- Initialize UDP Application --" << std::endl;
    simulation.InitializeUdpApplication();

    std::cout << "-- Starting Simulation --" << std::endl;

    // Set the max simulation time
    Simulator::Stop(Seconds(40.0));

    // Kick off the simulation
    Simulator::Run();
    Simulator::Destroy();

    std::cout << "-- Simulation Finished --" << std::endl;
}

/**
 * \brief Main function to run the Diffserv simulation or tests.
 * \param argc The number of command line arguments.
 * \param argv The command line arguments.
 * \returns 0 on success, 1 on failure.
 */
int main(int argc, char* argv[])
{
    std::string runMode;
    std::string configFile;

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
        std::cerr << "Error: --configFile must be set in sim mode\n";
        return 1;
        }

        RunSimulation (configFile);
    }
    // Otherwise, print an error message
    else
    {
        std::cerr << "Error: --runMode must be \"test\" or \"sim\"\n";
        return 1;
    }

    // Return 0 to indicate successful execution
    std::cout << "-- Program Finished --" << std::endl;
    return 0;
}
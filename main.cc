#include "diffserv-tests.h"
#include "simulation.h"
#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"
#include "ns3/simulator.h"
#include "ns3/core-module.h"

using namespace ns3;
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

    std::cout << "-- Initializing QoS Mechanism --" << std::endl;
    simulation.InitializeQosComponent();

    std::cout << "-- Configuring Topology for QoS --" << std::endl;
    simulation.CustomizeTopologyForQos();

    std::cout << "-- Initialize UDP Application --" << std::endl;
    simulation.InitializeUDPApplication();

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
    // Notify the user about the run mode and usage if not provided properly. 
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <runMode> [configFile]" << std::endl;
        return 1;
    }

    // Get the run mode from command line arguments
    // The first argument is the run mode (test or sim)
    std::string runMode = argv[1];

    // Check the run mode and execute the corresponding functionality
    if (runMode == "test")
    {
        std::cout << "-- Diffserv Tests --" << std::endl;

        DiffservTests diffServTests;
        diffServTests.RunAll();
    }
    // Check if the run mode is simulation
    else if (runMode == "sim")
    {
        // Check if the config file is provided
        if (argc < 3)
        {
            std::cerr << "Missing config file for simulation mode." << std::endl;
            return 1;
        }

        // Get the config file from command line arguments
        std::string configFile = argv[2];

        // Run the simulation with the provided config file
        std::cout << "-- Simulation Mode --" << std::endl;
        RunSimulation(configFile);
    }
    else
    {
        // If the run mode is not recognized, print an error message
        std::cerr << "Unknown run mode: " << runMode << std::endl;
        return 1;
    }

    // Return 0 to indicate successful execution
    std::cout << "-- Program Finished --" << std::endl;
    std::cout << "Exiting..." << std::endl;
    return 0;
}
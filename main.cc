#include "diffserv-tests.h"
#include "simulation.h"            // <-- declares InitializeTopology
#include <iostream>
#include <fstream>
#include <string>
#include "json.hpp"
#include "ns3/simulator.h"
#include "ns3/core-module.h"  // (optional but often useful)

using namespace ns3;
using json = nlohmann::json;

void RunSimulation(const std::string& configFile)
{
    Simulation sim;

    if (sim.parseConfigs(configFile))
    {
        std::cerr << "Failed to parse QoS config." << std::endl;
        return;
    }
    else
    {
        std::cout << "-- Parsed QoS Config --" << std::endl;
        sim.PrintConfig();
    }

    std::cout << "-- Initializing QoS Mechanism --" << std::endl;
    sim.InitializeQOSMechanism();

    std::cout << "-- Building Topology --" << std::endl;
    sim.InitializeTopology();

    std::cout << "-- Starting UDP Applications --" << std::endl;
    sim.InitializeUDPApplication();

    std::cout << "-- Starting Simulation --" << std::endl;
    Simulator::Stop(Seconds(40.0));
    Simulator::Run();
    std::cout << "-- Simulation Time: " << Simulator::Now().GetSeconds() << " seconds" << std::endl;
    std::cout << "-- Simulation Statistics --" << std::endl;
    std::cout << "  - Total Packets Sent: " << sim.data.max_packets[0] + sim.data.max_packets[1] << std::endl;
    std::cout << "  - Total Packets Received: " << sim.data.max_packets[0] + sim.data.max_packets[1] << std::endl;

    std::cout << "-- Simulation Finished --" << std::endl;
    std::cout << "-- Cleaning Up --" << std::endl;
    Simulator::Destroy();
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <runMode> [configFile]" << std::endl;
        return 1;
    }

    std::string runMode = argv[1];

    if (runMode == "test")
    {
        std::cout << "-- Diffserv Tests --" << std::endl;
        DiffservTests tests;
        tests.RunAll();
    }
    else if (runMode == "sim")
    {
        if (argc < 3)
        {
            std::cerr << "Missing config file for simulation mode." << std::endl;
            return 1;
        }

        std::string configFile = argv[2];
        std::cout << "-- Simulation Mode --" << std::endl;
        RunSimulation(configFile);
    }
    else
    {
        std::cerr << "Unknown run mode: " << runMode << std::endl;
        return 1;
    }

    return 0;
}
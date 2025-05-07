#include "simulation.h"
#include "filter.h"
#include <fstream>
#include "json.hpp"
#include "destination-port-number.h"
#include <filesystem>

// Include the necessary headers for JSON parsing
using json = nlohmann::json;

namespace ns3 {

    // Define constants for packet size and interval
    const Time Simulation::PACKET_TRANS_INTERVAL = Seconds(0.002);

    // Define constants for the stop, start and interval times
    static constexpr double STOP_TIME        = 50.0;
    static constexpr double SERVER_START     = 1.0;
    static constexpr double CLIENT_START_OFFSETS[] = { 2.0, 10.0 };

    /** 
     * 
     * \brief Enable pcap tracing for the simulation.
     * This function sets up pcap tracing for the point-to-point links
     * and the network devices.
     * \param sched The scheduler type (SPQ or DRR).
     */    
    std::pair<std::string, std::string> Simulation::BuildPcapFileNames(const std::string& sched)
    {
        // Use std::filesystem to handle file paths
        namespace fs = std::filesystem;

        // Get the current time and format it for the filename
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::ostringstream timestamp;
        timestamp << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
        
        // Get just a string from the timestamp
        std::string ts = timestamp.str();

        // Get the current working directory
        fs::path ccDir   = fs::path(__FILE__).parent_path();

        // Get the directory for pcap files
        fs::path pcapDir = ccDir / "pcaps";

        // Create the Pre and Post pcap file names
        std::string preName  = (pcapDir / ("Pre_"  + sched + "_" + ts)).string();
        std::string postName = (pcapDir / ("Post_" + sched + "_" + ts)).string();

        // Return the pcap file names
        return { preName, postName };
    }

    /**
     * \brief Parses the configuration file and initializes QoS data.
     * \param configFileName The path to the configuration file.
     * \returns true if parsing fails, false otherwise.
     */
    bool Simulation::parseConfigs(const std::string& configFileName)
    {
        // Open the configuration file
        // Use std::ifstream to read the file
        std::ifstream inputFile(configFileName);

        // Check if the file opened successfully
        // If not, print an error message and return
        if (!inputFile)
        {
            NS_LOG_UNCOND("Unable to open config file: " << configFileName);
            return true;
        }

        // Create a JSON object to hold the parsed data
        json configInput;

        // Use the json library to parse the file
        try {
            inputFile >> configInput;
        
        // Handle any exceptions that may occur during parsing
        } catch (const std::exception& e) {
            NS_LOG_UNCOND("Error parsing config file: " << e.what());
            return true;
        }

        // Check if the required keys are present in the JSON object
        // If not, print an error message and return
        if (!configInput.contains("QoS") || !configInput["QoS"].contains("Queues")) {
            NS_LOG_UNCOND("Invalid config file format: Missing QoS or Queues key");
            return true;
        }

        // Initialize the QoS data structure (Type is either SPQ or DRR)
        qosConfig.qosType = configInput["QoS"]["Type"];

        // Set the attribute values for the queues
        for (const auto& queue : configInput["QoS"]["Queues"]) {

            qosConfig.maxPackets.push_back(queue["MaxPackets"]);
            qosConfig.destinationPorts.push_back(queue["DestPort"]);
            qosConfig.defaults.push_back(queue["Default"]);

            // Check if the queue has a priority or weight attribute and add it to the respective attribute
            // This is done using the type of the queue (SPQ or DRR)
            if (qosConfig.qosType == "SPQ") {
                qosConfig.priorities.push_back(queue["Priority"]);

            } else if (qosConfig.qosType == "DRR") {
                qosConfig.weights.push_back(queue["Weight"]);
            }
        }

        // Set the count of queues
        // This is the size of the destination ports vector
        qosConfig.queueCount = qosConfig.destinationPorts.size();

        return false;
    }

    /**
     * \brief Prints the QoS configuration to the console.
     * This function prints the QoS type, queue attributes, and other relevant information.
     */
    void Simulation::PrintConfig() const
    {
        // Print the QoS configuration
        NS_LOG_UNCOND("QoS Configuration:");
        NS_LOG_UNCOND("  Scheduler Type: " << qosConfig.qosType);
        NS_LOG_UNCOND("  Queue Count:    " << qosConfig.queueCount);

        // Print the queue attributes
        for (uint32_t i = 0; i < qosConfig.queueCount; ++i) {
            NS_LOG_UNCOND("  Queue " << i + 1 << ":");
            NS_LOG_UNCOND("    MaxPackets: " << qosConfig.maxPackets[i]);
            NS_LOG_UNCOND("    DestPort:   " << qosConfig.destinationPorts[i]);
            NS_LOG_UNCOND("    Default:    " << (qosConfig.defaults[i] ? "true" : "false"));
            // Print the priority or weight based on the QoS type
            // This is done using the type of the queue (SPQ or DRR)
            if (qosConfig.qosType == "SPQ") {
                NS_LOG_UNCOND("    Priority:   " << qosConfig.priorities[i]);
            } else if (qosConfig.qosType == "DRR") {
                NS_LOG_UNCOND("    Weight:     " << qosConfig.weights[i]);
            }
        }
    }
    /**
     * \brief Initializes the DRR queue scheduler.
     * This function creates an instance of the DRR class and populates it with the parsed data.
     */
    void Simulation::InitializeDrr()
    {
        // Create an instance of the DRR class
        drr = CreateObject<DRR>();

        // Set the queue filters and traffic class values
        for (int i = 0; i < qosConfig.queueCount; i++)
        {
            // Create a filter element for the destination port
            DestinationPortNumber *destinationPortFilterElement = new DestinationPortNumber(qosConfig.destinationPorts[i]);

            // Create a filter and add the destination port filter element to it
            Filter *filter = new Filter();
            filter->AddFilterElement(destinationPortFilterElement);

            // Create a traffic class and set its attributes
            TrafficClass *trafficClass = new TrafficClass();

            // Set the maximum packets, weight, and default flag for the traffic class
            trafficClass->SetMaxPackets(qosConfig.maxPackets[i]);
            trafficClass->SetWeight(qosConfig.weights[i]);
            trafficClass->SetIsDefault(qosConfig.defaults[i]);

            // Add the filter to the traffic class
            // This is done to match the packets against the filter
            trafficClass->AddFilter(filter);

            // Add the traffic class to the DRR queue scheduler
            // This is done to set up the queue scheduler with the traffic classes
            drr->RegisterQueue(trafficClass);
        }
    }

    /**
     * \brief Initializes the QoS mechanism based on the configuration.
     * This function creates instances of the queue scheduler and populates them with the parsed data.
     */
    void Simulation::InitializeQosScheduler()
    {
        // Check the QoS type and initialize the corresponding queue scheduler
        if (qosConfig.qosType == "DRR") {
            // Initialize the DRR queue scheduler
            InitializeDrr();
        }
        else if (qosConfig.qosType == "SPQ") {
            // Initialize the SPQ queue scheduler
            InitializeSpq();
        }
        else
            // If the QoS type is not recognized, print an error message
            NS_FATAL_ERROR("Unknown QoS type: " << qosConfig.qosType);
    }

    /**
     * \brief Initializes the SPQ queue scheduler.
     * This function creates an instance of the SPQ class and populates it with the parsed data.
     */
    void Simulation::InitializeSpq()
    {
        // Create an instance of the SPQ class
        spq = CreateObject<SPQ>();

        // Set the queue filters and traffic class values
        for (int i = 0; i < qosConfig.queueCount; ++i) {
            DestinationPortNumber* destinationPortFilterElement = new DestinationPortNumber(qosConfig.destinationPorts[i]);

            // Create a filter and add the destination port filter element to it
            // This is done to match the packets against the filter
            Filter* filter = new Filter();
            filter->AddFilterElement(destinationPortFilterElement);

            // Create a traffic class and set its attributes
            TrafficClass* trafficClass = new TrafficClass();

            // Set the max packets, priority level, and default flag for the traffic class
            trafficClass->SetMaxPackets(qosConfig.maxPackets[i]);
            trafficClass->SetPriorityLevel(qosConfig.priorities[i]);
            trafficClass->SetIsDefault(qosConfig.defaults[i]);

            // Add the filter to the traffic class
            trafficClass->AddFilter(filter);

            // Add the traffic class to the SPQ queue scheduler
            // This is done to set up the queue scheduler with the traffic classes
            spq->RegisterQueue(trafficClass);
        }
    }

    /**
     * \brief Customizes the topology based on the QoS type.
     * This function sets up the queue scheduler for the second link (router0 to node1).
     */
    void Simulation::InitializeUdpApplication()
    {
        // Get the point-to-point network device for the second link (router0 to node1)
        Ptr<PointToPointNetDevice> link1PtpNetworkDevice = router0->GetDevice(1)->GetObject<PointToPointNetDevice>();

        // Set the queue scheduler for the second link based on the QoS type
        if (qosConfig.qosType == "SPQ") {
            link1PtpNetworkDevice->SetQueue(spq);

            // Build UDP Application for SPQ
            InitializeSpqUdpApplication();
        }
        
        // Set to DRR if the QoS type is DRR
        else if (qosConfig.qosType == "DRR") {
            // Set the queue scheduler for the second link to DRR
            link1PtpNetworkDevice->SetQueue(drr);

            // Build UDP Application for DRR
            InitializeDrrUdpApplication();
        }
    }

    /**
     * \brief Initializes the network topology.
     * Creates nodes, sets up point-to-point links, and assigns IP addresses.
     */
    void Simulation::InitializeTopology()
    {
        // Set up nodes and router
        node0 = CreateObject<Node>();
        node1 = CreateObject<Node>();
        router0 = CreateObject<Node>();

        // Add components to node container for all node container
        allNodesContainer = NodeContainer(node0, router0, node1);

        // Set up link nodes
        NodeContainer link0Nodes(node0, router0);
        NodeContainer link1Nodes(router0, node1);

        // Set up point-to-point links (per pdf requirements)
        link0Ptp.SetDeviceAttribute("DataRate", StringValue("4Mbps"));
        link0Ptp.SetChannelAttribute("Delay", StringValue("10ms"));
        link1Ptp.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
        link1Ptp.SetChannelAttribute("Delay", StringValue("10ms"));

        // Install point-to-point devices
        networkDevice0 = link0Ptp.Install(link0Nodes);
        networkDevice1 = link1Ptp.Install(link1Nodes);

        // Install internet stack on all nodes
        // This is required for IP address assignment and routing
        InternetStackHelper stack;
        stack.Install(allNodesContainer);

        // Set up IP addresses for the devices
        Ipv4AddressHelper link0IpSubnet, link1IpSubnet;
        link0IpSubnet.SetBase("10.1.1.0", "255.255.255.0");
        link1IpSubnet.SetBase("10.1.2.0", "255.255.255.0");

        // Assign IP addresses to the devices
        networkDevice0Interface = link0IpSubnet.Assign(networkDevice0);
        networkDevice1Interface = link1IpSubnet.Assign(networkDevice1);

        // Set up routing
        Ipv4GlobalRoutingHelper::PopulateRoutingTables();

        // Set up the queue scheduler for the second link (router0 to node1)
        // This is where we set the queue scheduler for the second link
        Ptr<PointToPointNetDevice> link1PtpNetworkDevice = router0->GetDevice(1)->GetObject<PointToPointNetDevice>();
    }

    /**
     * \brief Initializes the UDP applications for DRR.
     * This function sets up the UDP server and client applications based on the QoS type.
     */
    void Simulation::InitializeDrrUdpApplication()
    {
       // Install all the servers on node n1 in a loop
            // n1 is the destination node
            ApplicationContainer serverApplications;

            // Loop through the number of queues and create a server for each
            for (uint32_t i = 0; i < qosConfig.queueCount; ++i)
            {
                UdpServerHelper server(qosConfig.destinationPorts[i]);

                // Install the server on node n1 for each queue
                auto _application = server.Install(allNodesContainer.Get(2));

                // Set the server to start and stop at specific times
                _application.Start(Seconds(SERVER_START));
                _application.Stop(Seconds(STOP_TIME));

                // Add the server application to the container
                serverApplications.Add(_application);
            }

            // Install all the clients on node n0 in a loop
            ApplicationContainer clientApps;

            // Loop through the number of queues and create a client for each
            // n0 is the source node
            for (uint32_t i = 0; i < qosConfig.queueCount; ++i)
            {
                UdpClientHelper client(networkDevice1Interface.GetAddress(1), qosConfig.destinationPorts[i]);

                // Set the client attributes
                client.SetAttribute("MaxPackets", UintegerValue(qosConfig.maxPackets[i]));
                client.SetAttribute("Interval",  TimeValue(PACKET_TRANS_INTERVAL));
                client.SetAttribute("PacketSize", UintegerValue(PACKET_SIZE));

                // Install the client on node n0
                auto apps = client.Install(allNodesContainer.Get(0));

                // Set the client to start and stop at specific times
                apps.Start(Seconds(CLIENT_START_OFFSETS[0]));
                apps.Stop(Seconds(STOP_TIME));

                // Add the client application to the container
                // This is done to set up the client applications for each queue
                clientApps.Add(apps);
            }

            // Get the file names for pcap tracing
            auto [preName, postName] = BuildPcapFileNames(qosConfig.qosType);

            // Enable pcap tracing for the point-to-point links
            link0Ptp.EnablePcap(preName,  networkDevice0.Get(1));
            link1Ptp.EnablePcap(postName, networkDevice1.Get(0));
    }

    /**
     * \brief Initializes the UDP applications for SPQ.
     * This function sets up the UDP server and client applications based on the QoS type.
     */
    void Simulation::InitializeSpqUdpApplication()
    {
        // For each destination port, install a UDP server and client
        for (size_t i = 0; i < qosConfig.destinationPorts.size(); ++i)
        {
            uint32_t port = qosConfig.destinationPorts[i];

            // Install the server on node n1
            // n1 is the destination node
            {
                // create & install in one line
                auto apps = UdpServerHelper(port).Install(node1);
                apps.Start(Seconds(SERVER_START));
                apps.Stop (Seconds(STOP_TIME));
            }

            // Set up UDP Client Helper
            // This is used to create a UDP client application
            UdpClientHelper client( networkDevice1Interface.GetAddress(1), port);

            // Set the client attributes
            client.SetAttribute("MaxPackets", UintegerValue(qosConfig.maxPackets[i]));
            client.SetAttribute("Interval",   TimeValue    (PACKET_TRANS_INTERVAL));
            client.SetAttribute("PacketSize", UintegerValue(PACKET_SIZE));

            // Install the client on node n0
            {
                auto apps = client.Install(node0);
                apps.Start(Seconds(CLIENT_START_OFFSETS[i]));
                apps.Stop (Seconds(STOP_TIME));
            }
        }

        // Get the file names for pcap tracing
        auto [preName, postName] = BuildPcapFileNames(qosConfig.qosType);

        // Enable pcap tracing for the point-to-point links
        link0Ptp.EnablePcap(preName,  networkDevice0.Get(1));
        link1Ptp.EnablePcap(postName, networkDevice1.Get(0));
    }
} // namespace ns3
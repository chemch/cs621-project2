#include "simulation.h"
#include "filter.h"
#include <fstream>
#include "json.hpp"
#include "destination-port-number.h"

// Include the necessary headers for JSON parsing
using json = nlohmann::json;

namespace ns3 {

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
            std::cerr << "Failed to open config file: " << configFileName << std::endl;
            return true;
        }

        // Create a JSON object to hold the parsed data
        json configInput;

        // Use the json library to parse the file
        try {
            inputFile >> configInput;
        
        // Handle any exceptions that may occur during parsing
        } catch (const std::exception& e) {
            std::cerr << "Failed to parse JSON: " << e.what() << std::endl;
            return true;
        }

        // Check if the required keys are present in the JSON object
        // If not, print an error message and return
        if (!configInput.contains("QoS") || !configInput["QoS"].contains("Queues")) {
            std::cerr << "Invalid config provided. Check your config file." << std::endl;
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
        std::cout << "QoS Configuration:" << std::endl;
        std::cout << "  Scheduler Type: " << qosConfig.qosType << std::endl;

        // Print the queue attributes
        for (uint32_t i = 0; i < qosConfig.queueCount; ++i) {
            std::cout << "  Queue " << i + 1 << ":" << std::endl;
            std::cout << "    MaxPackets: " << qosConfig.maxPackets[i] << std::endl;
            std::cout << "    DestPort:   " << qosConfig.destinationPorts[i] << std::endl;
            std::cout << "    Default:    " << (qosConfig.defaults[i] ? "true" : "false") << std::endl;

            // Print the priority or weight based on the QoS type
            // This is done using the type of the queue (SPQ or DRR)
            if (qosConfig.qosType == "SPQ") {
                std::cout << "    Priority:   " << qosConfig.priorities[i] << std::endl;
            } else if (qosConfig.qosType == "DRR") {
                std::cout << "    Weight:     " << qosConfig.weights[i] << std::endl;
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
            drr->AddQueue(trafficClass);
        }
    }

    /**
     * \brief Initializes the QoS mechanism based on the configuration.
     * This function creates instances of the queue scheduler and populates them with the parsed data.
     */
    void Simulation::InitializeQosComponent()
    {
        // Check the QoS type and initialize the corresponding queue scheduler
        if (qosConfig.qosType == "DRR")
            InitializeDrr();

        else if (qosConfig.qosType == "SPQ")
            InitializeSpq();

        else
            // If the QoS type is not recognized, print an error message
            // and terminate the program
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
            spq->AddQueue(trafficClass);
        }
    }

    /**
     * \brief Customizes the topology based on the QoS type.
     * This function sets up the queue scheduler for the second link (router0 to node1).
     */
    void Simulation::CustomizeTopologyForQos()
    {
        // Get the point-to-point network device for the second link (router0 to node1)
        Ptr<PointToPointNetDevice> link1PtpNetworkDevice = router0->GetDevice(1)->GetObject<PointToPointNetDevice>();

        // Set the queue scheduler for the second link based on the QoS type
        if (qosConfig.qosType == "SPQ")
            link1PtpNetworkDevice->SetQueue(spq);
        
        // Set to DRR if the QoS type is DRR
        else if (qosConfig.qosType == "DRR")
            link1PtpNetworkDevice->SetQueue(drr);
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

        //     if (data.name == "SPQ")
        //     link1PtpNetworkDevice->SetQueue(spq);
        // else if (data.name == "DRR")
        //     link1PtpNetworkDevice->SetQueue(drr);
    }

    void Simulation::InitializeUDPApplication()
    {
        uint32_t maxPacketSize = 1000;
        Time interPacketInterval = Seconds(0.002);

        // Add timestamp to pcap file names
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::ostringstream timestamp;
        timestamp << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");

        if (qosConfig.qosType == "SPQ") {
            UdpServerHelper server1(qosConfig.destinationPorts[0]);
            UdpServerHelper server2(qosConfig.destinationPorts[1]);

            auto apps1 = server1.Install(node1);
            apps1.Start(Seconds(1.0));
            apps1.Stop(Seconds(40.0));

            auto apps2 = server2.Install(node1);
            apps2.Start(Seconds(1.0));
            apps2.Stop(Seconds(40.0));

            UdpClientHelper client1(networkDevice1Interface.GetAddress(1), qosConfig.destinationPorts[0]);
            client1.SetAttribute("MaxPackets", UintegerValue(qosConfig.maxPackets[0]));
            client1.SetAttribute("Interval", TimeValue(interPacketInterval));
            client1.SetAttribute("PacketSize", UintegerValue(maxPacketSize));

            UdpClientHelper client2(networkDevice1Interface.GetAddress(1), qosConfig.destinationPorts[1]);
            client2.SetAttribute("MaxPackets", UintegerValue(qosConfig.maxPackets[1]));
            client2.SetAttribute("Interval", TimeValue(interPacketInterval));
            client2.SetAttribute("PacketSize", UintegerValue(maxPacketSize));

            apps1 = client1.Install(node0);
            apps1.Start(Seconds(2.0));
            apps1.Stop(Seconds(40.0));

            apps2 = client2.Install(node0);
            apps2.Start(Seconds(14.0));
            apps2.Stop(Seconds(40.0));

            link0Ptp.EnablePcap("scratch/diffserv/pcaps/Pre_SPQ_" + timestamp.str(), networkDevice0.Get(1));
            link1Ptp.EnablePcap("scratch/diffserv/pcaps/Post_SPQ_" + timestamp.str(), networkDevice1.Get(0));
        }
        else if (qosConfig.qosType == "DRR") {

            // create UdpClient applications on n0
            uint32_t maxPacketSize = 1000;
            Time interPacketInterval = Seconds(0.002);


            // ----------------------------------------------------------------
            // 1) Install all your servers on node n1 (all.Get(2)) in a loop
            // ----------------------------------------------------------------
            ApplicationContainer serverApps;
            for (uint32_t i = 0; i < qosConfig.queueCount; ++i)
            {
                UdpServerHelper server(qosConfig.destinationPorts[i]);
                auto apps = server.Install(allNodesContainer.Get(2));
                apps.Start(Seconds(1.0));
                apps.Stop(Seconds(40.0));
                serverApps.Add(apps);
            }

            // ----------------------------------------------------------------
            // 2) Install all your clients on node n0 (all.Get(0)) in a loop
            // ----------------------------------------------------------------
            ApplicationContainer clientApps;
            for (uint32_t i = 0; i < qosConfig.queueCount; ++i)
            {
                UdpClientHelper client(networkDevice1Interface.GetAddress(1), qosConfig.destinationPorts[i]);
                client.SetAttribute("MaxPackets", UintegerValue(qosConfig.maxPackets[i]));
                client.SetAttribute("Interval",  TimeValue(interPacketInterval));
                client.SetAttribute("PacketSize", UintegerValue(maxPacketSize));

                auto apps = client.Install(allNodesContainer.Get(0));
                apps.Start(Seconds(2.0));
                apps.Stop(Seconds(40.0));
                clientApps.Add(apps);
            }

            link0Ptp.EnablePcap("scratch/diffserv/pcaps/Pre_DRR_" + timestamp.str(), networkDevice0.Get(1));
            link1Ptp.EnablePcap("scratch/diffserv/pcaps/Post_DRR_" + timestamp.str(), networkDevice1.Get(0));
        }
    }
} // namespace ns3
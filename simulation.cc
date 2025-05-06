#include "simulation.h"
#include "filter.h"
#include <fstream>
#include "json.hpp"
#include "destination-port-number.h"

using json = nlohmann::json;

namespace ns3 {

bool Simulation::parseConfigs(const std::string& filename)
{
    std::ifstream inFile(filename);
    if (!inFile)
    {
        std::cerr << "Failed to open config file: " << filename << std::endl;
        return true;
    }

    json config;
    try {
        inFile >> config;
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse JSON: " << e.what() << std::endl;
        return true;
    }

    if (!config.contains("QoS") || !config["QoS"].contains("Queues")) {
        std::cerr << "Invalid config structure." << std::endl;
        return true;
    }

    data.name = config["QoS"]["Name"];

    for (const auto& q : config["QoS"]["Queues"]) {
        data.max_packets.push_back(q["MaxPackets"]);
        data.dest_ports.push_back(q["DestPort"]);
        data.defaults.push_back(q["Default"]);

        if (data.name == "SPQ") {
            data.priorities.push_back(q["Priority"]);
        } else if (data.name == "DRR") {
            data.weights.push_back(q["Weight"]);
        }
    }

    data.count = data.dest_ports.size();
    return false;
}

void Simulation::PrintConfig() const
{
    std::cout << "QoS Configuration:" << std::endl;
    std::cout << "  Scheduler Type: " << data.name << std::endl;

    for (uint32_t i = 0; i < data.count; ++i) {
        std::cout << "  Queue " << i + 1 << ":" << std::endl;
        std::cout << "    MaxPackets: " << data.max_packets[i] << std::endl;
        std::cout << "    DestPort:   " << data.dest_ports[i] << std::endl;
        std::cout << "    Default:    " << (data.defaults[i] ? "true" : "false") << std::endl;

        if (data.name == "SPQ") {
            std::cout << "    Priority:   " << data.priorities[i] << std::endl;
        } else if (data.name == "DRR") {
            std::cout << "    Weight:     " << data.weights[i] << std::endl;
        }
    }
}

void Simulation::InitializeQOSMechanism()
{
    if (data.name == "SPQ")
        InitializeSPQ();
    else if (data.name == "DRR")
        InitializeDRR();
    else
        NS_FATAL_ERROR("Unknown QoS type: " << data.name);
}

void Simulation::InitializeSPQ()
{
    spq = CreateObject<SPQ>();
    for (int i = 0; i < data.count; ++i) {
        DestinationPortNumber* dstport_fe = new DestinationPortNumber(data.dest_ports[i]);
        Filter* f = new Filter();
        f->addFilterElement(dstport_fe);

        TrafficClass* tc = new TrafficClass();
        tc->SetMaxPackets(data.max_packets[i]);
        tc->SetPriorityLevel(data.priorities[i]);
        tc->SetIsDefault(data.defaults[i]);
        tc->AddFilter(f);

        spq->AddQueue(tc);

        // print queue information
        std::cout << "Queue " << i + 1 << ": MaxPackets=" << data.max_packets[i]
                  << ", DestPort=" << data.dest_ports[i]
                  << ", Priority=" << data.priorities[i]
                  << ", Default=" << (data.defaults[i] ? "true" : "false") << std::endl;
    }
}

void Simulation::InitializeDRR()
{
    // create instance of object
    drr = CreateObject<DRR>();

    // populate DRR class
    for (int i = 0; i < data.count; i++)
    {
        // create and add filters
        DestinationPortNumber *dstport_fe = new DestinationPortNumber(data.dest_ports[i]);
        Filter *f1 = new Filter();
        f1->addFilterElement(dstport_fe);

        // add traffic class values
        TrafficClass *trafficClass = new TrafficClass();

        trafficClass->SetMaxPackets(data.max_packets[i]);
        trafficClass->SetWeight(data.weights[i]);
        trafficClass->SetIsDefault(data.defaults[i]);
        trafficClass->AddFilter(f1);

        // add traffic class to drr
        drr->AddQueue(trafficClass);
    }

    // print queue information
    for (int i = 0; i < data.count; ++i) {
        std::cout << "Queue " << i + 1 << ": MaxPackets=" << data.max_packets[i]
                  << ", DestPort=" << data.dest_ports[i]
                  << ", Weight=" << data.weights[i]
                  << ", Default=" << (data.defaults[i] ? "true" : "false") << std::endl;
    }
}

void Simulation::InitializeTopology()
{
    n0 = CreateObject<Node>();
    r = CreateObject<Node>();
    n1 = CreateObject<Node>();
    all = NodeContainer(n0, r, n1);

    NodeContainer crNodes(n0, r);
    NodeContainer rsNodes(r, n1);

    pointToPoint1.SetDeviceAttribute("DataRate", StringValue("4Mbps"));
    pointToPoint1.SetChannelAttribute("Delay", StringValue("10ms"));
    pointToPoint2.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
    pointToPoint2.SetChannelAttribute("Delay", StringValue("10ms"));

    devices1 = pointToPoint1.Install(crNodes);
    devices2 = pointToPoint2.Install(rsNodes);

    InternetStackHelper stack;
    stack.Install(all);

    Ipv4AddressHelper address1, address2;
    address1.SetBase("10.1.1.0", "255.255.255.0");
    address2.SetBase("10.1.2.0", "255.255.255.0");

    interfaces1 = address1.Assign(devices1);
    interfaces2 = address2.Assign(devices2);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    Ptr<PointToPointNetDevice> crDevice = r->GetDevice(1)->GetObject<PointToPointNetDevice>();
    if (data.name == "SPQ")
        crDevice->SetQueue(spq);
    else if (data.name == "DRR")
        crDevice->SetQueue(drr);

    // print out important information
    std::cout << "Node 0 IP: " << interfaces1.GetAddress(0) << std::endl;
    std::cout << "Node 1 IP: " << interfaces1.GetAddress(1) << std::endl;
    std::cout << "Router IP: " << interfaces2.GetAddress(0) << std::endl;
}

void Simulation::InitializeUDPApplication()
{
    uint32_t maxPacketSize = 1000;
    Time interPacketInterval = Seconds(0.002);
    // uint32_t maxPacketCount = 15000;

     // Add timestamp to pcap file names
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::ostringstream timestamp;
    timestamp << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");

    if (data.name == "SPQ") {
        UdpServerHelper server1(data.dest_ports[0]);
        UdpServerHelper server2(data.dest_ports[1]);

        auto apps1 = server1.Install(n1);
        apps1.Start(Seconds(1.0));
        apps1.Stop(Seconds(40.0));

        auto apps2 = server2.Install(n1);
        apps2.Start(Seconds(1.0));
        apps2.Stop(Seconds(40.0));

        UdpClientHelper client1(interfaces2.GetAddress(1), data.dest_ports[0]);
        client1.SetAttribute("MaxPackets", UintegerValue(data.max_packets[0]));
        client1.SetAttribute("Interval", TimeValue(interPacketInterval));
        client1.SetAttribute("PacketSize", UintegerValue(maxPacketSize));

        UdpClientHelper client2(interfaces2.GetAddress(1), data.dest_ports[1]);
        client2.SetAttribute("MaxPackets", UintegerValue(data.max_packets[1]));
        client2.SetAttribute("Interval", TimeValue(interPacketInterval));
        client2.SetAttribute("PacketSize", UintegerValue(maxPacketSize));

        apps1 = client1.Install(n0);
        apps1.Start(Seconds(2.0));
        apps1.Stop(Seconds(40.0));

        apps2 = client2.Install(n0);
        apps2.Start(Seconds(14.0));
        apps2.Stop(Seconds(40.0));

        // print out important information
        // std::cout << "Node 0 IP: " << interfaces1.GetAddress(0) << std::endl;
        // std::cout << "Node 1 IP: " << interfaces1.GetAddress(1) << std::endl;
        // std::cout << "Router IP: " << interfaces2.GetAddress(0) << std::endl;
        // std::cout << "Client 1 Destination: " << interfaces2.GetAddress(1) << ":" << data.dest_ports[0] << std::endl;
        // std::cout << "Client 2 Destination: " << interfaces2.GetAddress(1) << ":" << data.dest_ports[1] << std::endl;
        // std::cout << "Client 1 MaxPackets: " << maxPacketCount << std::endl;
        // std::cout << "Client 2 MaxPackets: " << maxPacketCount << std::endl;
        // std::cout << "Client 1 PacketSize: " << maxPacketSize << std::endl;
        // std::cout << "Client 2 PacketSize: " << maxPacketSize << std::endl;
        // std::cout << "Client 1 Interval: " << interPacketInterval.GetSeconds() << " seconds" << std::endl;
        // std::cout << "Client 2 Interval: " << interPacketInterval.GetSeconds() << " seconds" << std::endl;

        pointToPoint1.EnablePcap("scratch/diffserv/pcaps/Pre_SPQ_" + timestamp.str(), devices1.Get(1));
        pointToPoint2.EnablePcap("scratch/diffserv/pcaps/Post_SPQ_" + timestamp.str(), devices2.Get(0));
    }
    else if (data.name == "DRR") {
        // create UdpServer applications on n1
        UdpServerHelper server1(data.dest_ports[0]);
        UdpServerHelper server2(data.dest_ports[1]);

        ApplicationContainer apps1 = server1.Install(all.Get(2));
        apps1.Start(Seconds(1.0));
        apps1.Stop(Seconds(40.0));
        ApplicationContainer apps2 = server2.Install(all.Get(2));
        apps2.Start(Seconds(1.0));
        apps2.Stop(Seconds(40.0));

        // create UdpClient applications on n0
        uint32_t maxPacketSize = 1000;
        Time interPacketInterval = Seconds(0.002);
        uint32_t maxPacketCount = 1000;

        UdpClientHelper client1(interfaces2.GetAddress(1), data.dest_ports[0]);
        client1.SetAttribute("MaxPackets", UintegerValue(maxPacketCount));
        client1.SetAttribute("Interval", TimeValue(interPacketInterval));
        client1.SetAttribute("PacketSize", UintegerValue(maxPacketSize));

        UdpClientHelper client2(interfaces2.GetAddress(1), data.dest_ports[1]);
        client2.SetAttribute("MaxPackets", UintegerValue(maxPacketCount));
        client2.SetAttribute("Interval", TimeValue(interPacketInterval));
        client2.SetAttribute("PacketSize", UintegerValue(maxPacketSize));

        apps1 = client1.Install(all.Get(0));
        apps1.Start(Seconds(2.0));
        apps1.Stop(Seconds(40.0));

        apps2 = client2.Install(all.Get(0));
        apps2.Start(Seconds(2.0));
        apps2.Stop(Seconds(40.0));

        pointToPoint1.EnablePcap("scratch/diffserv/pcaps/Pre_DRR_" + timestamp.str(), devices1.Get(1));
        pointToPoint2.EnablePcap("scratch/diffserv/pcaps/Post_DRR_" + timestamp.str(), devices2.Get(0));
    }
}

} // namespace ns3

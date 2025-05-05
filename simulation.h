#pragma once

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

#include "spq.h"
#include "drr.h"

namespace ns3 {

struct QosData {
    std::string name;
    std::vector<uint32_t> max_packets;
    std::vector<uint32_t> dest_ports;
    std::vector<uint32_t> priorities;  // used if SPQ
    std::vector<uint32_t> weights;     // used if DRR
    std::vector<double> intervals;
    std::vector<uint32_t> packet_sizes;
    std::vector<bool> defaults;
    uint32_t count;
};

class Simulation {
public:
    // Parsed QoS data
    QosData data;

    // Queue scheduler instances
    Ptr<SPQ> spq;
    Ptr<DRR> drr;

    bool parseConfigs(const std::string& filename);
    void PrintConfig() const;

    // Top-level initializer based on QoS name
    void InitializeQOSMechanism();

    // Queue scheduler construction
    void InitializeSPQ();
    void InitializeDRR();

    // Network and app setup
    void InitializeTopology();
    void InitializeUDPApplication();

private:
    // Node and topology
    Ptr<Node> n0, r, n1;
    NodeContainer all;
    NetDeviceContainer devices1, devices2;
    Ipv4InterfaceContainer interfaces1, interfaces2;

    // Link helpers
    PointToPointHelper pointToPoint1, pointToPoint2;
};

} // namespace ns3
#include <string>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/net-device.h"           

#include "spq.h"
#include "drr.h"

namespace ns3 {

    /**
     * \brief Structure to hold QoS data.
     * This structure is used to parse the configuration file and
     * initialize the QoS mechanism.
     */
    struct QosConfiguration {
        // QoS type (SPQ or DRR)
        std::string qosType;

        // Maximum packets for each queue
        std::vector<uint32_t> maxPackets;

        // Destination port for each queue
        std::vector<uint32_t> destinationPorts;

        // SPQ specific priority levels
        std::vector<uint32_t> priorities;

        // DRR specific weights
        std::vector<uint32_t> weights; 

        // Default queue flags
        // true if the queue is default, false otherwise
        // This is used to determine which queue to use when no other matches
        std::vector<bool> defaults;

        // Number of queues
        uint32_t queueCount;
    };

    class Simulation {
        public:
            // Parsed QoS data
            QosConfiguration qosConfig;

            // Queue scheduler instances
            Ptr<SPQ> spq;
            Ptr<DRR> drr;

            // Handler for JSON parsing
            bool parseConfigs(const std::string& configFileName);

            // Print the parsed configuration
            void PrintConfig() const;

            // Initialize the topology
            void InitializeTopology();

            // Set up Qos scheduler (SPQ or )
            void InitializeQosScheduler();

            // Queue scheduler customization
            // This function sets up the queue scheduler for the second link (router0 to node1)
            void InitializeUdpApplication();

        private:
            // Set constant values for packet size and interval
            static constexpr uint32_t PACKET_SIZE = 1000;
            static const Time PACKET_TRANS_INTERVAL;

            // Node and topology
            Ptr<Node> node0, router0, node1;
            NodeContainer allNodesContainer;
            NetDeviceContainer networkDevice0, networkDevice1;

            // IP address containers
            // These are used to assign IP addresses to the devices
            Ipv4InterfaceContainer networkDevice0Interface, networkDevice1Interface;

            // Point-to-point links 
            // Link 0 is between node0 and router
            // Link 1 is between router and node1
            PointToPointHelper link0Ptp, link1Ptp;

            // Enable pcap tracing
            // This function enables pcap tracing for the point-to-point links
            static std::pair<std::string, std::string> BuildPcapFileNames(const std::string& sched);

            // Network and app setup
            void InitializeSpqUdpApplication();
            void InitializeDrrUdpApplication();

            // Queue scheduler construction
            void InitializeSpq();
            void InitializeDrr();
    };

} // namespace ns3
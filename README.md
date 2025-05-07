# Table of Contents
* [Overview](#overview)
* [Developer](#overview)
* [Requirements](#requirements)
* [Run Instructions](#run-instructions)
* [Functionality Overview](#functionality-overview)
* [Limitations](#limitations)
* [Citations](#citations)
* [UML Deviations](#uml-deviations)
* [Config Files](#config-files)
* [Validation](#validation)
* [Design Considerations](#design-considerations)

---
# Overview

This project extends the NS-3 network simulator with a modular Differentiated Services (DiffServ) framework. At its core is a DiffServ abstract base class—derived from ns3::Queue<Packet>—that models packet classification and scheduling. Incoming packets are sorted into one of many TrafficClass queues via a configurable set of FilterElement rules (e.g., source/destination IP or port, protocol number). The base class provides a Classify(pkt) hook to assign packets to queues and a pure-virtual Schedule() method to pick which queue’s head-of-line packet should be served next. By overriding the protected DoEnqueue/DoDequeue/DoRemove/DoPeek hooks, DiffServ integrates seamlessly with NS-3’s built-in drop-tail, tracing, and callback machinery.

Building on this foundation, two concrete QoS mechanisms are implemented: Strict Priority Queuing (SPQ) and Deficit Round Robin (DRR). The SPQ subclass simply implements Schedule() to always drain the non-empty queue with the highest priority (lowest priority number). The DRR subclass maintains per-queue byte-deficit counters (quantums), updates them round-robin based on configured weights, and overrides AddQueue() and DoDequeue() to keep its internal deficit state in sync. Both SPQ and DRR accept external configuration via JSON files specifying queue counts, priorities or quantum values, and filter rules. Finally, a suite of automated NS-3 simulation scripts runs a three-node topology (4 Mbps → 1 Mbps) under bulk-transfer workloads to validate that SPQ enforces strict priority and DRR achieves weighted fairness as expected.

---
# Developer

- Chase Chemero (USFCA)

--- 
# Requirements

- NS3 (v3.42) - Recommend Using the All-in-One Tarball (https://www.nsnam.org/releases/ns-3-42/)
- Niels Lohmann's Json for C++ (v3.12.0) (Bundled w/ this Application ;-)) (https://nlohmann.me>)
- Python3 (>= v3.13.3)
- CLang (Version for Mac): arm64-apple-darwin24.4.0
- CMake (>= 4.0.1)

--- 
# Run Instructions


---
# Functionality Overview

---
# Limitations
- This implementation meets all the project specifications and validation requirements as outlined. 
- If pushed to Production/integrated w/ NS3 then we would likely want to update Simulation to allow for n number of SPQ TrafficClasses. The current implementation only supports 2 (which is specified in the PDF requirements).
--- 
# Citations


---
# UML Deviations

- <b>DiffServ Classify()</b> is not set as a pure virtual method as the DiffServ Base class is able to implement concrete functionality required by all base classes. Forcing sub-classes to each implement the same logic would be wasteful and violate the DRY principle. 
- <b>DiffServe AddQueue()</b> was added to allow all sub classes to add new Traffic Classes (Queues) to the q_class vector. This should be on the UML Diagram as most subclasses benefit from centralized queue management. Note: This could've also been done with GetQueues which likely all sub classes would need.
- <b>TrafficClass Get/Set<Member>()</b> has Getters/Setters added for the Filter Elements. This allows the configuration attributes to be applied to the queue as it is being created or modified. 
- <b><All Classes> Match()</b> have had their method changed to have the first letter (M) capitalized. NS3 follows a standard convention of having the first letter of Methods capitalized but the UML diagram shows the method as 'match', not 'Match. Not sure if this is a typo, but NS3 convention dictates 'Match'.

---
# Config Files
- Json was used for configuration files. For each DiffServ QOS Mechanism(SPQ and DRR) there 2 config files. 
- The primary validation files are generated using the first config files (spq-config-1.json and drr-config-2.json). The secondary config files were for testing more complex scenarios like best effort class starvation. These configs are just for examining the queue behaviors on edge case input. 
- Config files follow this format: (Note: You need either Weight or Priority depending on Type)

```json
{
    "QoS": {
      "Type": "<DRR or SPQ>",
      "Queues": [
        {
          "no": 1,
          "MaxPackets": <Max Packets You Want for this TrafficClass>,
          "Weight": <Quantum for DRR, Ignored for SPQ>(Optional),
          "Priority": <Integer Priority Where Lower is Better>(Optional),
          "Default": <Set as Default Queue for UnMatched>,
          "DestPort": <FilterElement>
        },
        {
          "no": 2,
          "MaxPackets": <Max Packets You Want for this TrafficClass>,
          "Weight": <Quantum for DRR, Ignored for SPQ>(Optional),
          "Priority": <Integer Priority Where Lower is Better>(Optional),
          "Default": <Set as Default Queue for UnMatched>,
          "DestPort": <FilterElement>
        }
      ]
    }
  }
```

--- 
# Validation

---
# Design Considerations


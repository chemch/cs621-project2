#pragma once

#include <string>

namespace ns3 {
    /**
     * \ingroup diffserv
     * \brief Run the simulation based on the provided configuration file.
     * \param configFile The path to the configuration file.
     */
    void RunSimulation (const std::string &configFile);
} // namespace ns3
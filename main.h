#ifndef CS621_PROJECT2_MAIN_H
#define CS621_PROJECT2_MAIN_H

#include <string>

namespace ns3 {
  class Simulation;
}

/// Runs the DiffServ sim given a JSON config path.
void RunSimulation(const std::string& configFile);

#endif // CS621_PROJECT2_MAIN_H
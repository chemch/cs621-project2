#ifndef DRR_H
#define DRR_H

#include "diff-serv.h"
#include <vector>

namespace ns3 {

/**
 * \ingroup diffserv
 * \brief Deficit Round Robin (DRR) scheduler extending DiffServ
 *
 * This class implements the DRR scheduling algorithm, which distributes
 * bandwidth across multiple traffic classes according to their weight.
 */
class DRR : public DiffServ
{
public:
    DRR();

    /**
     * \brief Dequeue the packet selected by the DRR scheduling logic.
     * \returns Ptr<Packet> dequeued packet or nullptr if none available.
     */
    Ptr<Packet> Dequeue() override;

    /**
     * \brief Select the next packet to be dequeued based on DRR.
     * \returns Ptr<const Packet> packet at the front of the scheduled queue.
     */
    Ptr<const Packet> Schedule() const override;

    /**
     * \brief Add a new TrafficClass to the DRR queue system.
     * \param trafficClass pointer to the TrafficClass instance.
     */
    void AddQueue(TrafficClass* trafficClass) override;

private:
    uint32_t active_queue; ///< Currently active queue index
    mutable uint32_t next_active_queue; ///< Candidate queue during scheduling
    std::vector<uint32_t> deficit_counter; ///< Deficit counters for each queue
    mutable std::vector<uint32_t> next_deficit_counter; ///< Working copy during Schedule
};

} // namespace ns3

#endif // DRR_H
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
        ~DRR() override = default;

        /**
         * \brief Dequeue the packet selected by the DRR scheduling logic.
         * \returns Ptr<Packet> dequeued packet or nullptr if none available.
         */
        Ptr<Packet> Dequeue() override;

        /**
         * \brief Remove the next packet from the active queue.
         * \returns Ptr<Packet> removed packet or nullptr if none available.
         */
        Ptr<Packet> Remove() override;

        /**
         * \brief Select the next packet to be dequeued based on DRR.
         * \returns Ptr<const Packet> packet at the front of the scheduled queue.
         */
        Ptr<const Packet> Schedule() const override;

        /**
         * \brief Add a new TrafficClass to the DRR.
         * \param trafficClass pointer to the TrafficClass instance.
         */
        void RegisterQueue(TrafficClass* trafficClass) override;

    private:
        // The current queue index being served
        uint32_t currentQueue;

        // The next queue index to be served
        mutable uint32_t nextQueue; 

        // The official quantum for each queue that has been committed
        std::vector<uint32_t> queueQuantum; 

        // This is a working array to keep track of the quantum for each queue while searching for the next candidate queue
        mutable std::vector<uint32_t> tempDeficitCounter;

        /**
         * \brief Get the list of queues in the DiffServ class.
         * \returns A vector of pointers to TrafficClass objects.
         */
        std::vector<TrafficClass*> GetQueues() const;
    };
} // namespace ns3

#endif // DRR_H
#ifndef SPQ_H
#define SPQ_H

#include "diff-serv.h"
#include "ns3/packet.h"

namespace ns3 {
    /**
     * \ingroup diffserv
     * \brief SPQ (Strict Priority Queue) implementation of DiffServ scheduler.
     */
    class SPQ : public DiffServ
    {
        public:
            SPQ();
            ~SPQ() override = default;

            /**
             * \brief Schedules the next packet to be dequeued based on strict priority.
             * \return A pointer to the next scheduled packet. (Override of the base class method)
             */
            Ptr<const Packet> Schedule() const override;
        
        private:
            /**
             * \brief Get the list of queues in the DiffServ class.
             * \returns A vector of pointers to TrafficClass objects.
             */
            std::vector<TrafficClass*> GetQueues() const;
    };
} // namespace ns3

#endif // SPQ_H
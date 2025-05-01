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

    /**
     * \brief Schedules the next packet to be dequeued based on strict priority.
     * \returns A pointer to the next scheduled packet. (Override of the base class method)
     */
    Ptr<const Packet> Schedule() const override;
};

} // namespace ns3

#endif // SPQ_H
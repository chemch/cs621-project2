#ifndef DIFF_SERV_H
#define DIFF_SERV_H

#include "traffic-class.h"
#include "ns3/queue.h"
#include "ns3/packet.h"

namespace ns3 {

/**
 * \ingroup diffserv
 * \brief DiffServ class to manage packets and queues.
 */
class DiffServ : public Queue<Packet>
{

public:
    DiffServ();

       /**
     * \brief Add a queue to the DiffServ class.
     * \param queue The TrafficClass object to be added.
     */
    virtual void AddQueue(TrafficClass* queue);

    /**
     * \brief Enqueue a packet into the DiffServ queue.
     */
    bool Enqueue(Ptr<Packet> pkt) override;

    /** 
     * \brief Dequeue a packet from the DiffServ queue.
     * \returns The dequeued packet.
     */
    Ptr<Packet> Dequeue() override;

    /** 
     * \brief Remove a packet from the DiffServ queue.
     * \returns The removed packet.
     */
    Ptr<Packet> Remove() override;

    /**
     * \brief Peek at the front packet in the DiffServ queue.
     * \returns The front packet without removing it.
     * \note This function does not modify the queue.
     */
    Ptr<const Packet> Peek() const override;

    /**
     * Primary Functions for DiffServ ( Schedule, Classify )
     */

    /**
     * \brief Classify a packet based on its properties.
     * \returns The classification result.
     */
    virtual uint32_t Classify(Ptr<Packet> pkt);

    /**
     * \brief Schedule the next packet to be sent.
     * \returns The scheduled packet.
     */
    virtual Ptr<const Packet> Schedule() const = 0;


protected:
    std::vector<TrafficClass*> GetQueues() const;

private:
    std::vector<TrafficClass*> q_class;

    Ptr<const Packet> ExecutePeek() const;
    bool ExecuteEnqueue(Ptr<Packet> pkt);
    Ptr<Packet> ExecuteDequeue();
    Ptr<Packet> ExecuteRemove();
    
};

} // namespace ns3

#endif // DIFF_SERV_H
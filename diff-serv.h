#ifndef DIFF_SERV_H
#define DIFF_SERV_H

#include "ns3/queue.h"
#include "traffic_class.h"

using namespace ns3;

class DiffServ : public Queue<Packet>
{
    public:
        DiffServ();

        bool Enqueue(Ptr<Packet> p) override;
        Ptr<Packet> Dequeue() override;
        Ptr<Packet> Remove() override;
        Ptr<const Packet> Peek() const override;
        virtual Ptr<const Packet> Schedule() const = 0;
        virtual uint32_t Classify(Ptr<Packet> p);
        virtual void AddQueue(TrafficClass *q);

    protected:
        std::vector<TrafficClass*> GetQueues() const;

    private:
        std::vector<TrafficClass*> q_class;

        bool DoEnqueue(Ptr<Packet> p);
        Ptr<Packet> DoDequeue();
        Ptr<Packet> DoRemove();
        Ptr<const Packet> DoPeek() const;
};

#endif // DIFF_SERV_H

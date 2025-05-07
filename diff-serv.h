#ifndef DIFF_SERV_H
#define DIFF_SERV_H

#include <vector>
#include "ns3/log.h"
#include "ns3/packet.h"
#include "traffic-class.h"
#include "ns3/queue.h"

namespace ns3 {
    /**
     * \ingroup diffserv
     * \brief DiffServ class to manage packets and queues.
     */
    class DiffServ : public Queue<Packet>
    {
        public:
            /**
             * \brief Constructor for DiffServ class.
             * \details Initializes the DiffServ class and sets up the logging component.
             */
            DiffServ();

            /**
             * \brief Destructor for DiffServ class.
             * \details Cleans up the DiffServ class and releases resources.
             */
            virtual ~DiffServ() = default;

            /**
             * \brief Add a queue to the DiffServ class.
             * \param queue The TrafficClass object to be added.
             */
            virtual void RegisterQueue(TrafficClass* queue);

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
             */
            Ptr<Packet> Remove() override;

            /**
             * \brief Peek at the front packet in the DiffServ queue.
             * \returns The front packet without removing it.
             * \note This function does not modify the queue.
             */
            Ptr<const Packet> Peek() const override;

            /**
             * \brief Classify a packet based on its properties.
             * \returns The classification result.
             * \note This function is supposed to have '= 0' meaning it is a pure virtual function and it needs to be implemented in derived classes.
             * However, it is not a pure virtual function in this case, as it is defined in the base class. The functionality for Classify would be the same
             * in most cases, so there is really no need to have it as a pure virtual function. This is documented in my report as an improvement on the diffserv class. 
             */
            virtual uint32_t Classify(Ptr<Packet> pkt);

            /**
             * \brief Schedule the next packet to be sent.
             * \returns The scheduled packet.
             */
            virtual Ptr<const Packet> Schedule() const = 0;

        protected:
            std::vector<TrafficClass*> q_class;

            // Called by Queue<Packet>::Enqueue()
            bool DoEnqueue (Ptr<Packet> pkt);

            // Called by Queue<Packet>::Dequeue()
            Ptr<Packet> DoDequeue ();

            // Called by Queue<Packet>::Remove()
            Ptr<Packet> DoRemove ();

            // Called by Queue<Packet>::Peek()
            Ptr<const Packet> DoPeek () const;
    };
} // namespace ns3

#endif // DIFF_SERV_H
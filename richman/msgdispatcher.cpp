#include "msgdispatcher.h"
#include "msg/msgreceiver.h"
#include "msg/msgsender.h"
#include "models/msgcomm.h"
#include <cmath>
#include <iostream>
#include "utils/distributedstream.h"

#include "utils/appdebug.h"

using namespace MsgComm;

MsgDispatcher::MsgDispatcher(AtomicRichmanInfo &parentData, const TunnelMap &tunnels, int richmansAmount):
    parentData(parentData), tunnels(tunnels), id(parentData.getId())
{
    this->walkers.resize(richmansAmount);
    for(int id=0; id < richmansAmount; id++) {
        this->walkers[id] = id;
    }
}

void MsgDispatcher::run()
{
    MsgReceiver recv(this->id, this->walkers);
    while(true) {
        this->print("Waiting for msg");
        Packet p = recv.wait(MsgSourceTag::Walker);

        this->print("Execute operation");
        this->executeOperation(p);

        this->printTunnels();
    }
}

void MsgDispatcher::printTunnels()
{
    #ifdef APP_DEBUG_TUNNELS
    for(auto [id, tunnel]: this->tunnels) {
        std::string tqueue, tinside;
        for(auto t: tunnel->getQueue()) {
            tqueue += std::to_string(t.getId()) + " ";
        }

        for(auto t: tunnel->getInside()) {
            tinside += std::to_string(t) + " ";
        }
        std::string dir = describe(tunnel->getDirection());

        dstream.write("");
        dstream.write(dir);
        dstream.write("INSIDE: " + tinside);
        dstream.write("QUEUE: " + tqueue);
        dstream.write("");
    }
#endif
}

void MsgDispatcher::print(std::string str)
{
    #ifdef APP_DEBUG_DISPATCHER
    dstream.write("DISPATCHER: " + str);
    #endif
}

void MsgDispatcher::executeOperation(const Packet &packet)
{
    // decode operation type
    packet.getCmd([&packet, this](Request req) {
        this->handleRequest(packet, req);
    }, [](Response) {});
}

void MsgDispatcher::handleRequest(const Packet &packet, Request req)
{
    int tid = packet.getTunnel_id();
    Tunnel &tunnel = *this->tunnels[tid].get();
    const int expectant_id = packet.getData().getId();
    const int expectant_counter = packet.getData().getCounter();

    if(expectant_id != this->id) {
        int val = std::max(this->parentData.getCounter(), expectant_counter) + 1;
        this->parentData.setCounter(val);
    }

    switch(req) {
        case Request::Enter:
            if(tunnel.isQueueFilled()) {

                this->print("Deny push to queue " + describe(tunnel.getDirection()));
                if(expectant_id != this->id) {
                    this->parentData.incrementCounter();
                }

                MsgSender(this->id, expectant_id).send(Packet(Sender::Dispatcher,
                                                              Receiver::Walker,
                                                              Response::Deny,
                                                              this->parentData.getInfo(), tunnel.getTunnelId()));
            } else {
                tunnel.appendQueue(packet.getData()).sortQueue();
                this->print("Push to queue " + describe(tunnel.getDirection()));

                if(expectant_id != this->id) {
                    this->parentData.incrementCounter();
                }

                if(tunnel.isFirstInQueue(packet.getData()) && !tunnel.isTunnelFilled()) {
                    tunnel.removeFromQueue(expectant_id).insertTunnel(expectant_id);

                    this->print("Remove from queue, push to tunnel " + describe(tunnel.getDirection()));

                    MsgSender(this->id, expectant_id).send(Packet(Sender::Dispatcher, Receiver::Walker, Response::Accept,
                                                                  this->parentData.getInfo(), tunnel.getTunnelId()));
                } else {
                    MsgSender(this->id, expectant_id).send(Packet(Sender::Dispatcher, Receiver::Walker, Response::InQueue,
                                                                  this->parentData.getInfo(), tunnel.getTunnelId()));
                }
            } break;

        case Request::Exit:
        {
            tunnel.removeFromTunnel(expectant_id).removeFromQueue(expectant_id); // w kolejce lub w tunelu

            this->print("Remove from tunnel/queue " + describe(tunnel.getDirection()));
            if(expectant_id != this->id) {
                this->parentData.incrementCounter();
            }

            MsgSender(this->id, expectant_id).
                    send(Packet(Sender::Dispatcher, Receiver::Walker, Response::Accept,
                                this->parentData.getInfo(), tunnel.getTunnelId()));

            auto [first, ok] = tunnel.getFromQueue(0);
            if(ok && !tunnel.isTunnelFilled()) {
                tunnel.removeFromQueue(first.getId()).insertTunnel(first.getId());

                this->print("Push to tunnel " + describe(tunnel.getDirection()));
                if(expectant_id != this->id) {
                    this->parentData.incrementCounter();
                }

                MsgSender(this->id, first.getId()).send(Packet(Sender::Dispatcher, Receiver::Walker, Response::Accept,
                                                               this->parentData.getInfo(), tunnel.getTunnelId()));
            }

            } break;

        case Request::Status:
        {
            Response status;
            if(tunnel.isInsideTunnel(expectant_id)) {
                status = Response::Accept;
            } else if(tunnel.isInsideQueue(expectant_id)) {
                status = Response::InQueue;
            } else {
                throw std::runtime_error("invalid response status " + std::string(__FILE__) + " " + std::to_string(__LINE__));
            }

            MsgSender(this->id, expectant_id, MsgComm::MsgSourceTag::RollbackWalker)
                    .send(Packet(Sender::Dispatcher, Receiver::Walker, status,
                                 this->parentData.getInfo(), tunnel.getTunnelId()));

            } break;
    }
}

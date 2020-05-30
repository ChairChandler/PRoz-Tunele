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
    parentData(parentData), tunnels(tunnels), richmansAmount(richmansAmount), id(parentData.getId())
{
    this->allTargets.resize(richmansAmount);
    for(int id=0; id < richmansAmount; id++) {
        this->allTargets[id] = id;
    }

    otherDispatchers = this->allTargets;
    otherDispatchers.erase(std::remove_if(otherDispatchers.begin(), otherDispatchers.end(), [this](int v){return v == this->id;}));
}

void MsgDispatcher::run()
{
    MsgReceiver recv(this->parentData.getId(), this->allTargets);
    while(true) {
        dstream.write("Waiting for msg");
        Packet p = recv.wait();

        dstream.write("Execute operation");
        this->executeOperation(p);

        if(this->selfWalkerTunnelId != UNDEFINED) {
            dstream.write("Handle self");
            this->handleSelfWalker();
        }
    }
}

void MsgDispatcher::executeOperation(const Packet &packet)
{
    // decode operation type
    packet.getCmd([&packet, this](Request req) {
        this->handleRequest(packet, req);
    }, [&packet, this](Response res) {
        this->handleResponse(packet, res);
    });
}

void MsgDispatcher::handleRequest(const Packet &packet, Request req)
{
    Tunnel &tunnel = *this->tunnels[packet.getTunnel_id()].get();
    const int expectant_id = packet.getData().getId();
    const int expectant_counter = packet.getData().getCounter();

    if(expectant_id != this->id) {
        int val = std::max(this->parentData.getCounter(), expectant_counter);
        this->parentData.setCounter(val + 1);
        dstream.write("Clock after received = " + std::to_string(val + 1));
    }

    switch(req) {
        case Request::Enter:
            if(tunnel.isQueueFilled()) {

                dstream.write("Deny push to tunnel " + describe(tunnel.getDirection()));
                if(expectant_id != this->id) {
                    this->parentData.incrementCounter();
                    dstream.write("Clock after sent = " + std::to_string(this->parentData.getCounter()));
                }

                MsgSender(this->id, expectant_id).send(Packet(Sender::Dispatcher,
                                                              expectant_id == this->id ? Receiver::Walker : Receiver::Dispatcher,
                                                              Response::Deny,
                                                              this->parentData.getInfo(), tunnel.getTunnelId()));
            } else {
                tunnel.appendQueue(packet.getData()).sortQueue();

                if(expectant_id != this->id) {

                    if(tunnel.isFirstInQueue(packet.getData()) && !tunnel.isTunnelFilled()) {
                        tunnel.removeFromQueue(packet.getData()).insertTunnel(expectant_id);
                        this->parentData.incrementCounter();

                        dstream.write("Push another to tunnel " + describe(tunnel.getDirection()));
                        dstream.write("Clock after sent = " + std::to_string(this->parentData.getCounter()));
                        MsgSender(this->id, expectant_id).send(Packet(Sender::Dispatcher, Receiver::Dispatcher, Response::Enter,
                                                                      this->parentData.getInfo(), tunnel.getTunnelId()));
                    } else {
                        dstream.write("Push another to queue");
                    }
                } else {
                    this->selfWalkerTunnelId = tunnel.getTunnelId();

                    this->selfWalkerNegativeResponse = 0;
                    this->selfWalkerPositiveResponse = 0;

                    this->selfWalkerEnterRequest = true;
                    this->selfWalkerRichmanInfo = packet.getData();
                    dstream.write("Push self to queue"); // obsluga tego rzadania w handleSelf
                }
            }
        break;

        case Request::Exit:
            if(expectant_id != this->id) {
                tunnel.removeFromTunnel(expectant_id);
                this->parentData.incrementCounter();

                dstream.write("Remove another from tunnel " + describe(tunnel.getDirection()));
                dstream.write("Clock after sent = " + std::to_string(this->parentData.getCounter()));
            } else {
                this->selfWalkerTunnelId = tunnel.getTunnelId();

                this->selfWalkerNegativeResponse = 0;
                this->selfWalkerPositiveResponse = 0;

                dstream.write("Inform another about remove self from tunnel " + describe(tunnel.getDirection()));
                MsgSender(this->id, this->otherDispatchers).
                        send(Packet(Sender::Dispatcher, Receiver::Dispatcher, Request::Exit,
                                    this->parentData.getInfo(), tunnel.getTunnelId()));
            }


            auto [first, ok] = tunnel.getFromQueue(0);
            if(ok) {
                tunnel.removeFromQueue(first).insertTunnel(first.getId());

                if(this->id != first.getId()) {
                    this->parentData.incrementCounter();

                    dstream.write("Push another to tunnel " + describe(tunnel.getDirection()));
                    dstream.write("Clock after sent = " + std::to_string(this->parentData.getCounter()));
                    MsgSender(this->id, first.getId()).send(Packet(Sender::Dispatcher, Receiver::Dispatcher, Response::Enter,
                                                                   this->parentData.getInfo(), tunnel.getTunnelId()));
                } else {
                    dstream.write("Push self to tunnel " + describe(tunnel.getDirection()));
                    MsgSender(this->id, this->id).send(
                                Packet(Sender::Dispatcher, Receiver::Walker, Response::Enter,
                                       this->parentData.getInfo(), this->selfWalkerTunnelId));
                }
            }
        break;
    }
}

void MsgDispatcher::handleResponse(const Packet &packet, Response res)
{
    const int expectant_counter = packet.getData().getCounter();

    int counter = std::max(this->parentData.getCounter(), expectant_counter);
    this->parentData.setCounter(counter + 1);

    dstream.write("Clock after received = " + std::to_string(counter + 1));

    switch(res) {
        case Response::Enter:
            this->selfWalkerPositiveResponse++;
        break;

        case Response::Exit:
            this->selfWalkerPositiveResponse++;
        break;

        case Response::Deny:
            this->selfWalkerNegativeResponse++;
        break;
    }
}

void MsgDispatcher::handleSelfWalker()
{
    MsgSender walker(this->id, this->id);
    int amountSenders = this->richmansAmount - 1;

    Tunnel &tunnel = *this->tunnels[this->selfWalkerTunnelId].get();

    if(this->selfWalkerEnterRequest) {
        if(tunnel.isFirstInQueue(this->selfWalkerRichmanInfo) && !tunnel.isTunnelFilled()) {
            this->selfWalkerEnterRequest = false;
            dstream.write("Ask another for push self to tunnel " + describe(tunnel.getDirection()));
            MsgSender(this->id, this->otherDispatchers).
                    send(Packet(Sender::Dispatcher, Receiver::Dispatcher, Request::Enter,
                                this->parentData.getInfo(), tunnel.getTunnelId()));
        }
    }

    if(this->selfWalkerPositiveResponse == amountSenders) {

        if(tunnel.isInsideTunnel(this->id)) {
            tunnel.removeFromTunnel(this->id);

            dstream.write("Remove self from tunnel " + describe(tunnel.getDirection()));
            this->selfWalkerTunnelId = UNDEFINED;

            auto [first, ok] = tunnel.getFromQueue(0);
            if(ok) {
                tunnel.removeFromQueue(first);
                this->parentData.incrementCounter();

                dstream.write("Push another to tunnel " + describe(tunnel.getDirection()));
                dstream.write("Clock after sent = " + std::to_string(this->parentData.getCounter()));
                MsgSender(this->id, first.getId()).send(Packet(Sender::Dispatcher, Receiver::Dispatcher, Response::Enter,
                                                               this->parentData.getInfo(), tunnel.getTunnelId()));
            }
        } else if(!tunnel.isTunnelFilled()) {
                /*
                 * moze sie zdarzyc sytuacja w której otrzymam zgode od wszystkich lecz u mnie
                 * nie bede juz pierwszy w kolejce, miedzy wyslaniem rządania a odebraniem
                 * odpowiedzi ktoś mógł uzyskać pierwszeństwo w kolejce
                 *
                 * w takiej sytuacji muszę nagiąć zasady i wpuścić swojego do tunelu
                 * pomimo niespełnienia wymagań
                */

            tunnel.removeFromQueue(this->id).insertTunnel(this->id);
            this->selfWalkerTunnelId = UNDEFINED;

            dstream.write("Push self to tunnel " + describe(tunnel.getDirection()));
            walker.send(Packet(Sender::Dispatcher, Receiver::Walker, Response::Enter,
                               this->parentData.getInfo(), this->selfWalkerTunnelId));
        }

    } else if((this->selfWalkerNegativeResponse + this->selfWalkerPositiveResponse) == amountSenders) {
        tunnel.removeFromQueue(this->id);
        this->selfWalkerTunnelId = UNDEFINED;

        dstream.write("Deny self push to tunnel " + describe(tunnel.getDirection()));
        walker.send(Packet(Sender::Dispatcher, Receiver::Walker, Response::Deny,
                           this->parentData.getInfo(), this->selfWalkerTunnelId));
    }
}

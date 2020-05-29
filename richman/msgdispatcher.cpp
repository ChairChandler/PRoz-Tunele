#include "msgdispatcher.h"
#include "msg/msgreceiver.h"
#include "msg/msgsender.h"
#include "models/msgcomm.h"
#include <cmath>
#include <iostream>
#include "utils/distributedstream.h"

#include "utils/appdebug.h"

/* WAZNE!!! PO ODEBRANIU WIADOMOSCI OD SIEBIE NIE MOZNA ZWIEKSZAC JUZ LICZNIKA NAWET JAK WYSYLAMY
 * W PRZECIWNYM WYPADKU NASTAPI ZAKLESZCZENIE/BLEDNE PRZEPUSZCZANIE
 *
 * p1	[(id=1, clk=1), (id=0, clk=3)] przepuszcza 1

   p0	[(id=0, clk=1), (id=1, clk=3)] przepuszcza 0
 *
*/

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

        dstream.write("Handle self");
        this->handleSelfWalker();
    }
}

void MsgDispatcher::executeOperation(const Packet &packet)
{
    // decode operation type
    packet.getCmd([&packet, this](MsgComm::Request req) {
        this->handleRequest(packet, req);
    }, [&packet, this](MsgComm::Response res) {
        this->handleResponse(packet, res);
    });
}

void MsgDispatcher::handleRequest(const Packet &packet, MsgComm::Request req)
{
    Tunnel &tunnel = *this->tunnels[packet.getTunnel_id()].get();
    const int expectant_id = packet.getData().getId();
    const int expectant_counter = packet.getData().getCounter();

    int val = std::max(this->parentData.getCounter(), expectant_counter);
    this->parentData.setCounter(val + 1);

    dstream.write("Clock after received = " + std::to_string(val + 1));

    switch(req) {
        case MsgComm::Request::Enter:
            if(tunnel.isQueueFilled()) {
                this->parentData.incrementCounter();

                dstream.write("Deny push to tunnel");
                dstream.write("Clock after sent = " + std::to_string(this->parentData.getCounter()));
                MsgSender(this->id, expectant_id).send(Packet(MsgComm::Sender::Dispatcher,
                                                              expectant_id == this->id ? MsgComm::Receiver::Walker : MsgComm::Receiver::Dispatcher,
                                                              MsgComm::Response::Deny,
                                                              this->parentData.getInfo(), tunnel.getTunnelId()));
            } else {
                tunnel.appendQueue(packet.getData()).sortQueue();

                if(expectant_id != this->id) {

                    if(tunnel.isFirstInQueue(packet.getData()) && !tunnel.isTunnelFilled()) {
                        tunnel.insertTunnel(expectant_id);
                        this->parentData.incrementCounter();

                        dstream.write("Push another to tunnel");
                        dstream.write("Clock after sent = " + std::to_string(this->parentData.getCounter()));
                        MsgSender(this->id, expectant_id).send(Packet(MsgComm::Sender::Dispatcher,
                                                                      MsgComm::Receiver::Dispatcher,
                                                                      MsgComm::Response::Enter,
                                                                      this->parentData.getInfo(), tunnel.getTunnelId()));
                    }
                } else {
                    this->selfWalkerTunnelId = tunnel.getTunnelId();
                    this->parentData.incrementCounter();

                    dstream.write("Ask another for push self to tunnel");
                    dstream.write("Clock after sent = " + std::to_string(this->parentData.getCounter()));
                    MsgSender(this->id, this->otherDispatchers).
                            send(Packet(MsgComm::Sender::Dispatcher,
                                        MsgComm::Receiver::Dispatcher,
                                        MsgComm::Request::Enter,
                                        this->parentData.getInfo(), tunnel.getTunnelId()));
                }
            }
        break;

        case MsgComm::Request::Exit:
            if(expectant_id != this->id) {
                tunnel.removeFromTunnel(expectant_id);
                this->parentData.incrementCounter();

                dstream.write("Remove another from tunnel");
                dstream.write("Clock after sent = " + std::to_string(this->parentData.getCounter()));
                MsgSender(this->id, expectant_id).send(Packet(MsgComm::Sender::Dispatcher,
                                                              MsgComm::Receiver::Dispatcher,
                                                              MsgComm::Response::Exit,
                                                              this->parentData.getInfo(), tunnel.getTunnelId()));
            } else {
                this->selfWalkerTunnelId = tunnel.getTunnelId();
                this->parentData.incrementCounter();

                dstream.write("Inform another about remove self from tunnel");
                dstream.write("Clock after sent = " + std::to_string(this->parentData.getCounter()));
                MsgSender(this->id, this->otherDispatchers).
                        send(Packet(MsgComm::Sender::Dispatcher,
                                    MsgComm::Receiver::Dispatcher,
                                    MsgComm::Request::Exit,
                                    this->parentData.getInfo(), tunnel.getTunnelId()));
            }


            auto [first, ok] = tunnel.getFromQueue(0);
            if(ok) {
                this->parentData.incrementCounter();

                dstream.write("Push another to tunnel");
                dstream.write("Clock after sent = " + std::to_string(this->parentData.getCounter()));
                MsgSender(this->id, first.getId()).send(Packet(MsgComm::Sender::Dispatcher,
                                                               MsgComm::Receiver::Dispatcher,
                                                               MsgComm::Response::Enter,
                                                               this->parentData.getInfo(), tunnel.getTunnelId()));
            }
        break;
    }
}

void MsgDispatcher::handleResponse(const Packet &packet, MsgComm::Response res)
{
    const int expectant_counter = packet.getData().getCounter();

    int counter = std::max(this->parentData.getCounter(), expectant_counter);
    this->parentData.setCounter(counter + 1);

    dstream.write("Clock after received = " + std::to_string(counter + 1));

    switch(res) {
        case MsgComm::Response::Enter:
            this->selfWalkerPositiveResponse++;
        break;

        case MsgComm::Response::Exit:
            this->selfWalkerPositiveResponse++;
        break;

        case MsgComm::Response::Deny:
            this->selfWalkerNegativeResponse++;
        break;
    }
}

void MsgDispatcher::handleSelfWalker()
{
    MsgSender walker(this->id, this->id);
    int amountSenders = this->richmansAmount - 1;

    Tunnel &tunnel = *this->tunnels[this->selfWalkerTunnelId].get();

    if(this->selfWalkerPositiveResponse == amountSenders) {

        if(tunnel.isInsideTunnel(this->id)) {
            this->selfWalkerPositiveResponse = 0;
            tunnel.removeFromTunnel(this->id);

            dstream.write("Remove self from tunnel");
        } else if(!tunnel.isTunnelFilled()) {
                /*
                 * moze sie zdarzyc sytuacja w której otrzymam zgode od wszystkich lecz u mnie
                 * nie bede juz pierwszy w kolejce, miedzy wyslaniem rządania a odebraniem
                 * odpowiedzi ktoś mógł uzyskać pierwszeństwo w kolejce
                 *
                 * w takiej sytuacji muszę nagiąć zasady i wpuścić swojego do tunelu
                 * pomimo niespełnienia wymagań
                */

            this->selfWalkerPositiveResponse = 0;

            tunnel.removeFromQueue(this->id).insertTunnel(this->id);

            dstream.write("Push self to tunnel");
            this->parentData.incrementCounter();
            walker.send(Packet(MsgComm::Sender::Dispatcher,
                               MsgComm::Receiver::Walker,
                               MsgComm::Response::Enter,
                               this->parentData.getInfo(), this->selfWalkerTunnelId));
        }

    } else if((this->selfWalkerNegativeResponse + this->selfWalkerPositiveResponse) == amountSenders) {
        this->selfWalkerNegativeResponse = 0;
        this->selfWalkerPositiveResponse = 0;
        tunnel.removeFromQueue(this->id);

        dstream.write("Deny self push to tunnel");
        this->parentData.incrementCounter();
        walker.send(Packet(MsgComm::Sender::Dispatcher,
                           MsgComm::Receiver::Walker,
                           MsgComm::Response::Deny,
                           this->parentData.getInfo(), this->selfWalkerTunnelId));
    }
}

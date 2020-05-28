#include "msgdispatcher.h"
#include "msg/msgreceiver.h"
#include "msg/msgsender.h"
#include "models/msgcomm.h"
#include <cmath>
#include <iostream>
#include "utils/distributedstream.h"

#include "utils/appdebug.h"

MsgDispatcher::MsgDispatcher(AtomicRichmanInfo &parentData, const TunnelMap &tunnels, int richmansAmount):
    parentData(parentData), tunnels(tunnels), richmansAmount(richmansAmount)
{
    this->allTargets.resize(richmansAmount);
    for(int id=0; id < richmansAmount; id++) {
        this->allTargets[id] = id; // include self thread (walker + dispatcher)
    }

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
    Tunnel &tunnel = *this->tunnels[tunnel_id].get();
    const int expectant_id = data.getId();
    const int expectant_counter = data.getCounter();
    thread_local const int self_id = this->parentData.load().getId();

    thread_local MsgSender anotherIdSender(self_id, SpecificTarget::All, this->name);

    auto self = this->parentData.load();
    self.setCounter(std::max(self.getCounter(), expectant_counter));
    self.incrementCounter();
    this->parentData.store(self);

    this->writeStream("Clock after received = " + std::to_string(self.getCounter()));

    switch(type) {
        case Request::Enter:
            if(tunnel.isQueueFilled()) {
                this->parentData.store(this->parentData.load().incrementCounter());

                this->writeStream("Clock after sent = " + std::to_string(this->parentData.load().getCounter()));
                MsgSender(expectant_id, this->name).sendReply(Reply::Deny, this->parentData, tunnel_id);
            } else {
                tunnel.appendQueue(data).sortQueueByTime();
                if(expectant_id != self_id) {
                    if(tunnel.isFirstInQueue(data) && !tunnel.isTunnelFilled()) {
                        tunnel.insertTunnel(expectant_id);
                        this->parentData.store(this->parentData.load().incrementCounter());

                        this->writeStream("Clock after sent = " + std::to_string(this->parentData.load().getCounter()));
                        MsgSender(expectant_id, this->name).sendReply(Reply::Enter, this->parentData, tunnel_id);
                    }
                } else {
                    this->selfWalkerTunnelId = tunnel_id;
                    this->parentData.store(this->parentData.load().incrementCounter());

                    this->writeStream("Clock after sent = " + std::to_string(this->parentData.load().getCounter()));
                    anotherIdSender.sendRequest(Request::Enter, this->parentData, tunnel_id);
                }
            }
        break;

        case Request::Exit:
            if(expectant_id != self_id) {
                tunnel.removeFromTunnel(expectant_id);
                this->parentData.store(this->parentData.load().incrementCounter());

                this->writeStream("Clock after sent = " + std::to_string(this->parentData.load().getCounter()));
                MsgSender(expectant_id, this->name).sendReply(Reply::Exit, this->parentData, tunnel_id);

                auto [first, ok] = tunnel.getFromQueue(0);
                if(ok) {
                    this->parentData.store(this->parentData.load().incrementCounter());

                    this->writeStream("Clock after sent = " + std::to_string(this->parentData.load().getCounter()));
                    MsgSender(first.getId(), this->name).sendReply(Reply::Enter, this->parentData, tunnel_id);
                }
            } else {
                this->selfWalkerTunnelId = tunnel_id;
                this->parentData.store(this->parentData.load().incrementCounter());

                this->writeStream("Clock after sent = " + std::to_string(this->parentData.load().getCounter()));
                anotherIdSender.sendRequest(Request::Exit, this->parentData, tunnel_id);
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
    thread_local const int selfId = this->parentData.load().getId();
    thread_local MsgSender walker(selfId, this->name);
    thread_local const int amountSenders = this->richmansAmount - 1;

    Tunnel &tunnel = *this->tunnels[this->selfWalkerTunnelId].get();

    if(this->selfWalkerPositiveResponse == amountSenders) {
        this->selfWalkerPositiveResponse = 0;
        this->parentData.store(this->parentData.load().incrementCounter());

        if(tunnel.isInsideTunnel(selfId)) {
            tunnel.removeFromTunnel(selfId);
            walker.send(MsgCommResponse(MsgComm::ResDispatcherToWalker, Reply::Exit), this->parentData, this->selfWalkerTunnelId);
        } else {
            auto [first, ok] = tunnel.getFromQueue(0);
            if(ok && first.getId() == selfId) {
                tunnel.removeFromQueue(selfId).insertTunnel(selfId);
                walker.send(MsgCommResponse(MsgComm::ResDispatcherToWalker, Reply::Enter), this->parentData, this->selfWalkerTunnelId);
            } else {
                throw std::runtime_error("invalid queque state " + std::string(__FILE__) + " " +std::to_string(__LINE__));
            }
        }

    } else if((this->selfWalkerNegativeResponse + this->selfWalkerPositiveResponse) == amountSenders) {
        this->selfWalkerNegativeResponse = 0;
        this->selfWalkerPositiveResponse = 0;
        tunnel.removeFromQueue(selfId);
        this->parentData.store(this->parentData.load().incrementCounter());
        walker.send(MsgCommResponse(MsgComm::ResDispatcherToWalker, Reply::Deny), this->parentData, this->selfWalkerTunnelId);
    }
}

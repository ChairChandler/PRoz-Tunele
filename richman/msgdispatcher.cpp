#include "msgdispatcher.h"
#include "msg/msgreceiver.h"
#include "msg/msgsender.h"
#include <cmath>

MsgDispatcher::MsgDispatcher(std::atomic<RichmanInfo> &parentData, const TunnelMap &tunnels, int richmansAmount):
    parentData(parentData),
    tunnels(tunnels),
    richmansAmount(richmansAmount)
{
    std::vector<int> targets(richmansAmount);
    for(int id=0; id < richmansAmount; id++) {
        targets[id] = id; // self is shared between threads
    }
    MsgReceiver::setAllTarget(targets);
}

void MsgDispatcher::run()
{
    MsgReceiver recv(this->parentData.load().getId(), SpecificTarget::All);
    while(true) {
        Packet p = recv.wait();
        this->executeOperation(p);
        this->handleSelfWalker();
    }
}

void MsgDispatcher::executeOperation(Packet packet)
{
    struct Visit
    {
        MsgDispatcher &ds;
        Packet &p;
        Visit(MsgDispatcher &ds, Packet &p): ds(ds), p(p) {}
        void operator()(Request r)
        {
            ds.handleMsg(r, p.data, p.tunnel_id);
        }
        void operator()(Reply r)
        {
            ds.handleMsg(r, p.data);
        }
    };

    std::visit(Visit(*this, packet), packet.type);
}

void MsgDispatcher::handleMsg(Request type, RichmanInfo data, int tunnel_id)
{
    Tunnel &tunnel = *this->tunnels[tunnel_id].get();
    const int expectant_id = data.getId();
    const int expectant_counter = data.getCounter();
    static const int self_id = this->parentData.load().getId();

    static MsgSender anotherIdSender(self_id, SpecificTarget::All);

    auto self = this->parentData.load();
    self.setCounter(std::max(self.getCounter(), expectant_counter));
    self.incrementCounter();
    this->parentData.store(self);

    switch(type) {
        case Request::Enter:
            if(tunnel.isQueueFilled()) {
                this->parentData.store(this->parentData.load().incrementCounter());
                MsgSender(expectant_id).sendReply(Reply::Deny, this->parentData, tunnel_id);
            } else {
                tunnel.appendQueue(data).sortQueueByTime();
                if(expectant_id != self_id) {
                    if(tunnel.isFirstInQueue(data) && !tunnel.isTunnelFilled()) {
                        tunnel.insertTunnel(expectant_id);
                        this->parentData.store(this->parentData.load().incrementCounter());
                        MsgSender(expectant_id).sendReply(Reply::Enter, this->parentData, tunnel_id);
                    }
                } else {
                    this->selfWalkerTunnelId = tunnel_id;
                    anotherIdSender.sendRequest(Request::Enter, this->parentData, tunnel_id);
                }
            }
        break;

        case Request::Exit:
            if(expectant_id != self_id) {
                tunnel.removeFromTunnel(expectant_id);
                this->parentData.store(this->parentData.load().incrementCounter());
                MsgSender(expectant_id).sendReply(Reply::Exit, this->parentData, tunnel_id);
                auto [first, ok] = tunnel.getFromQueue(0);
                if(ok) {
                    this->parentData.store(this->parentData.load().incrementCounter());
                    MsgSender(first.getId()).sendReply(Reply::Enter, this->parentData, tunnel_id);
                }
            } else {
                this->selfWalkerTunnelId = tunnel_id;
                anotherIdSender.sendRequest(Request::Exit, this->parentData, tunnel_id);
            }
        break;
    }
}

void MsgDispatcher::handleMsg(Reply type, RichmanInfo data)
{
    const int expectant_counter = data.getCounter();

    auto self = this->parentData.load();
    self.setCounter(std::max(self.getCounter(), expectant_counter));
    self.incrementCounter();
    this->parentData.store(self);

    switch(type) {
        case Reply::Enter:
            this->selfWalkerPositiveResponse++;
        break;

        case Reply::Exit:
            this->selfWalkerPositiveResponse++;
        break;

        case Reply::Deny:
            this->selfWalkerNegativeResponse++;
        break;
    }
}

void MsgDispatcher::handleSelfWalker()
{
    static const int selfId = this->parentData.load().getId();
    static MsgSender walker(selfId);
    static const int amountSenders = this->richmansAmount - 1;

    Tunnel &tunnel = *this->tunnels[this->selfWalkerTunnelId].get();

    if(this->selfWalkerPositiveResponse == amountSenders) {
        this->selfWalkerPositiveResponse = 0;
        this->parentData.store(this->parentData.load().incrementCounter());

        if(tunnel.isInsideTunnel(selfId)) {
            tunnel.removeFromTunnel(selfId);
            walker.sendReply(Reply::Exit, this->parentData, this->selfWalkerTunnelId);
        } else {
            auto [first, ok] = tunnel.getFromQueue(0);
            if(ok && first.getId() == selfId) {
                tunnel.removeFromQueue(selfId).insertTunnel(selfId);
                walker.sendReply(Reply::Enter, this->parentData, this->selfWalkerTunnelId);
            } else {
                throw std::runtime_error("invalid queque state");
            }
        }

    } else if((this->selfWalkerNegativeResponse + this->selfWalkerPositiveResponse) == amountSenders) {
        this->selfWalkerNegativeResponse = 0;
        this->selfWalkerPositiveResponse = 0;
        tunnel.removeFromQueue(selfId);
        this->parentData.store(this->parentData.load().incrementCounter());
        walker.sendReply(Reply::Deny, this->parentData, this->selfWalkerTunnelId);
    }
}

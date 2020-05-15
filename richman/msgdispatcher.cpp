#include "msgdispatcher.h"
#include "msg/msgreceiver.h"

MsgDispatcher::MsgDispatcher(std::atomic<RichmanInfo> &parentData, size_t queueMaxSize, size_t tunnelMaxSize, int richmansAmount):
    parentData(parentData),
    earthTunnel(0, queueMaxSize, tunnelMaxSize, Place::Earth),
    dimTunnel(1, queueMaxSize, tunnelMaxSize, Place::Dimension),
    richmansAmount(richmansAmount)
{
    std::vector<int> targets(richmansAmount);
    for(int id=0; id < richmansAmount; id++) {
        targets[id] = id;
    }
    MsgReceiver::setAllTarget(targets);
}

void MsgDispatcher::run()
{
    static MsgReceiver recv(this->parentData.load().getId(), SpecificTarget::All);
    while(true) {
        Packet p = recv.wait();
        std::visit(this->handleMsg, p.type);
    }
}

void MsgDispatcher::executeOperation(Packet packet)
{

}

void MsgDispatcher::handleSelfWalker()
{

}

void MsgDispatcher::handleMsg(Request type)
{

}

void MsgDispatcher::handleMsg(Reply type)
{

}

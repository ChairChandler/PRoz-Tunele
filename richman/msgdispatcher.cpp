#include "msgdispatcher.h"

MsgDispatcher::MsgDispatcher(std::atomic<RichmanInfo> &parentData, size_t queueMaxSize, size_t tunnelMaxSize):
    parentData(parentData), earthTunnel(queueMaxSize, tunnelMaxSize, Place::Earth),
    dimTunnel(queueMaxSize, tunnelMaxSize, Place::Dimension)
{

}

void MsgDispatcher::run()
{

}

void MsgDispatcher::executeOperation(Packet packet)
{

}

void MsgDispatcher::handleSelfWalker()
{

}

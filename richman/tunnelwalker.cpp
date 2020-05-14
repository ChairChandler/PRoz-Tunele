#include "tunnelwalker.h"
#include "msg/msgsender.h"

TunnelWalker::TunnelWalker(std::atomic<RichmanInfo> &parentData, Place startingPlace):
    parentData(parentData), place(startingPlace)
{

}

void TunnelWalker::run()
{
    while(true) {
        Tunnel tunnel = this->enterTunnel();
        this->exitTunnel(tunnel);
        this->wait();
    }
}

Tunnel TunnelWalker::enterTunnel()
{
    static MsgSender walker(SpecificTarget::Self);
    //walker.sendRequest(Request::Enter, this->parentData, );
}

void TunnelWalker::wait()
{
    std::this_thread::sleep_for(this->waitTime);
}

void TunnelWalker::exitTunnel(Tunnel &tunnel)
{
    static MsgSender walker(SpecificTarget::Self);
}

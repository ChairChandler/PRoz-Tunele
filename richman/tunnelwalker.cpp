#include "tunnelwalker.h"
#include "msg/msgsender.h"
#include "msg/msgreceiver.h"
#include <stdexcept>
#include "richman.h"
#include <iostream>
#include "msg/distributedstream.h"

TunnelWalker::TunnelWalker(std::atomic<RichmanInfo> &parentData, Place startingPlace, const std::vector<Tunnel> &tInfo):
    parentData(parentData), place(startingPlace), tInfo(tInfo)
{

}

void TunnelWalker::run()
{
    while(true) {
        dstream.write("Enter tunnel");
        int tunnel_id = this->enterTunnel();
        this->wait();
        dstream.write("Exit tunnel");
        this->exitTunnel(tunnel_id);
        dstream.write("Wait");
        this->wait();
    }
}

int TunnelWalker::enterTunnel()
{
    static MsgSender walkerSend(SpecificTarget::Self);
    static MsgReceiver walkerRecv(this->parentData.load().getId(), SpecificTarget::Self);

    while(true) {
        for(const Tunnel &tInfo: this->tInfo) {
            if(tInfo.getDirection() == this->place) {
                continue;
            } else {
                int tid = tInfo.getTunnelId();
                this->parentData.store(this->parentData.load().incrementCounter());
                walkerSend.sendRequest(Request::Enter, this->parentData, tid);
                Packet p = walkerRecv.wait(MsgComm::ReplyRecvTag); dstream.write("RES"); //!!! ERROR
                switch(std::get<Reply>(p.type)) {
                    case Reply::Enter:
                        return tid;
                    break;

                    case Reply::Deny:
                        continue;
                    break;

                    default:
                        throw std::runtime_error("cannot handle reply code in walker");
                    break;
                }
            }
        }
    }
}

void TunnelWalker::wait()
{
    std::this_thread::sleep_for(this->waitTime);
}

void TunnelWalker::exitTunnel(int tunnel_id)
{
    static MsgSender walkerSend(SpecificTarget::Self);

    this->parentData.store(this->parentData.load().incrementCounter());
    walkerSend.sendRequest(Request::Exit, this->parentData, tunnel_id);
}

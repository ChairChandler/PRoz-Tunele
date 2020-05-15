#include "tunnelwalker.h"
#include "msg/msgsender.h"
#include "msg/msgreceiver.h"
#include <stdexcept>

TunnelWalker::TunnelWalker(std::atomic<RichmanInfo> &parentData, Place startingPlace, int tunnelsAmount):
    parentData(parentData), place(startingPlace), tunnelsAmount(tunnelsAmount)
{

}

void TunnelWalker::run()
{
    while(true) {
        int tunnel_id = this->enterTunnel();
        this->exitTunnel(tunnel_id);
        this->wait();
    }
}

int TunnelWalker::enterTunnel()
{
    static MsgSender walkerSend(SpecificTarget::Self);
    static MsgReceiver walkerRecv(this->parentData.load().getId(), SpecificTarget::Self);

    while(true) {
        for(int tid=0; tid < this->tunnelsAmount; tid++) {
            this->parentData.store(this->parentData.load().incrementCounter());
            walkerSend.sendRequest(Request::Enter, this->parentData, tid);
            Packet p = walkerRecv.wait(MsgComm::ReplyRecvTag);
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

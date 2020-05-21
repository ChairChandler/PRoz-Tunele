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
        this->writeStream("Enter tunnel");
        int tunnel_id = this->enterTunnel();

        this->wait();

        this->writeStream("Exit tunnel");
        this->exitTunnel(tunnel_id);

        this->writeStream("Wait");
        this->wait();
    }
}

int TunnelWalker::enterTunnel()
{
    thread_local MsgSender walkerSend(SpecificTarget::Self, this->name);
    thread_local MsgReceiver walkerRecv(this->parentData.load().getId(), SpecificTarget::Self, this->name);

    while(true) {
        for(const Tunnel &tInfo: this->tInfo) {
            if(tInfo.getDirection() == this->place) {
                continue;
            } else {
                int tid = tInfo.getTunnelId();
                this->parentData.store(this->parentData.load().incrementCounter());
                int myCounter = this->parentData.load().getCounter();

                this->writeStream("Clock after sent = " + std::to_string(myCounter));
                walkerSend.sendRequest(Request::Enter, this->parentData, tid);

                Packet p = walkerRecv.wait(MsgComm::ReplyRecvTag);

                int maxCounter = std::max(myCounter, p.data.getCounter());
                this->parentData.store(this->parentData.load().incrementCounter(maxCounter + 1));
                this->writeStream("Clock after received = " + std::to_string(this->parentData.load().getCounter()));

                switch(std::get<Reply>(p.type)) {
                    case Reply::Enter:
                        return tid;
                    break;

                    case Reply::Deny:
                        continue;
                    break;

                    default:
                        throw std::runtime_error("cannot handle reply code in walker " + std::string(__FILE__) + " " +std::to_string(__LINE__));
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
    thread_local MsgSender walkerSend(SpecificTarget::Self, this->name);

    this->parentData.store(this->parentData.load().incrementCounter());
    walkerSend.sendRequest(Request::Exit, this->parentData, tunnel_id);
}

void TunnelWalker::writeStream(const std::string &m)
{
    dstream.write(this->name, m);
}

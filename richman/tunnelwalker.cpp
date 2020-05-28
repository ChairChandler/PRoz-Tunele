#include "tunnelwalker.h"
#include "msg/msgsender.h"
#include "msg/msgreceiver.h"
#include <stdexcept>
#include "richman.h"
#include <iostream>
#include "utils/distributedstream.h"

TunnelWalker::TunnelWalker(AtomicRichmanInfo &parentData, Place startingPlace, const std::vector<Tunnel> &tInfo):
    parentData(parentData), place(startingPlace), tInfo(tInfo), id(parentData.getId())
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
    MsgSender walkerSender(this->id, this->id);
    MsgReceiver walkerReceiver(this->id, this->id);

    while(true) {
        for(const Tunnel &tInfo: this->tInfo) {
            if(tInfo.getDirection() == this->place) {
                continue;
            } else {
                int tid = tInfo.getTunnelId();
                int myCounter = this->parentData.incrementCounter().getCounter();

                dstream.write("Clock after sent = " + std::to_string(myCounter));
                walkerSender.send(Packet(MsgComm::Sender::Walker, MsgComm::Receiver::Dispatcher, MsgComm::Request::Enter, this->parentData.getInfo(), tid));

                Packet p = walkerReceiver.wait(MsgComm::MsgSourceTag::Dispatcher);

                int maxCounter = std::max(myCounter, p.getData().getCounter());
                this->parentData.setCounter(maxCounter + 1);

                dstream.write("Clock after received = " + std::to_string(this->parentData.getCounter()));
                p.getCmd([](MsgComm::Request){}, [tid](MsgComm::Response res){
                    switch(res) {
                        case MsgComm::Response::Enter:  return tid;
                        case MsgComm::Response::Deny:   break;
                        default:
                            throw std::runtime_error("cannot handle reply code in walker " + std::string(__FILE__) + " " + std::to_string(__LINE__));
                    }
                });
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
    this->parentData.incrementCounter();
    Packet p(MsgComm::Sender::Walker, MsgComm::Receiver::Dispatcher, MsgComm::Request::Exit, this->parentData.getInfo(), tunnel_id);
    MsgSender(this->id, this->id).send(p);
}

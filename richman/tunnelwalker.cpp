#include "tunnelwalker.h"
#include "msg/msgsender.h"
#include "msg/msgreceiver.h"
#include <stdexcept>
#include "richman.h"
#include <iostream>
#include "utils/distributedstream.h"

using namespace MsgComm;

TunnelWalker::TunnelWalker(AtomicRichmanInfo &parentData, Place startingPlace, const std::vector<Tunnel> &tInfo):
    parentData(parentData), destinationPlace(startingPlace), tunnels(tInfo), id(parentData.getId())
{

}

void TunnelWalker::run()
{
    while(true) {
        dstream.write("Choosing tunnel");
        int tunnel_id = this->enterTunnel();

        dstream.write("Enter tunnel");
        this->exitTunnel(tunnel_id);
        dstream.write("Exit tunnel");

        dstream.write("Wait");
        this->changePlace();
    }
}

int TunnelWalker::enterTunnel()
{
    while(true) {
        for(const Tunnel &tInfo: this->tunnels) {
            if(tInfo.getDirection() != this->destinationPlace) {
                int tid = tInfo.getTunnelId();
                this->sendReqToDispatcher(tid);
                Packet p = this->recvResFromDispatcher();

                if(this->isEnterResponse(p)) {
                    return tid;
                }
            }
        }
    }
}

bool TunnelWalker::isEnterResponse(Packet p)
{
    bool enter;
    p.getCmd([](Request){}, [&enter](Response res){
        switch(res) {
            case Response::Enter:
                enter = true;
            break;

            case Response::Deny:
                enter = false;
            break;

            default:
                throw std::runtime_error("cannot handle reply code in walker " + std::string(__FILE__) + " " + std::to_string(__LINE__));
        }
    });
    return enter;
}

void TunnelWalker::sendReqToDispatcher(int tunnel_id)
{
    MsgSender walkerSender(this->id, this->id);
    this->parentData.incrementCounter();

    dstream.write("Clock after sent = " + std::to_string(this->parentData.getCounter()));
    walkerSender.send(Packet(Sender::Walker, Receiver::Dispatcher, Request::Enter,
                             this->parentData.getInfo(), tunnel_id));
}

Packet TunnelWalker::recvResFromDispatcher()
{
    MsgReceiver walkerReceiver(this->id, this->id);
    Packet p = walkerReceiver.wait(MsgSourceTag::Dispatcher);

    int maxCounter = std::max(this->parentData.getCounter(), p.getData().getCounter());
    this->parentData.setCounter(maxCounter + 1);

    dstream.write("Clock after received = " + std::to_string(this->parentData.getCounter()));
    return p;
}

void TunnelWalker::exitTunnel(int tunnel_id)
{
    this->parentData.incrementCounter();
    Packet p(Sender::Walker, Receiver::Dispatcher, Request::Exit,
             this->parentData.getInfo(), tunnel_id);
    MsgSender(this->id, this->id).send(p);
}

void TunnelWalker::changePlace()
{
    this->destinationPlace = this->destinationPlace == Place::Earth ? Place::Dimension : Place::Earth;
}

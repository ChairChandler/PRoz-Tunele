#include "tunnelwalker.h"
#include "msg/msgsender.h"
#include "msg/msgreceiver.h"
#include <stdexcept>
#include "richman.h"
#include <iostream>
#include "utils/distributedstream.h"
#include <chrono>

using namespace MsgComm;

TunnelWalker::TunnelWalker(AtomicRichmanInfo &parentData, Place startingPlace, const std::vector<Tunnel> &tInfo, int richmansAmount):
    parentData(parentData), destinationPlace(startingPlace), tunnels(tInfo), id(parentData.getId()), dispatchersAmount(richmansAmount)
{
    this->dispatchers.resize(richmansAmount);
    for(int id=0; id < richmansAmount; id++) {
        this->dispatchers[id] = id;
    }
}

void TunnelWalker::run()
{
    while(true) {
        this->print("My place: " + describe(this->destinationPlace));
        this->print("Choosing tunnel");
        int tunnel_id = this->enterTunnel();

        this->print("Enter tunnel id: " + std::to_string(tunnel_id));
        this->exitTunnel(tunnel_id);
        this->print("Exit tunnel id: " + std::to_string(tunnel_id));

        this->print("Wait");
        this->changePlace();
    }
}

int TunnelWalker::enterTunnel()
{
    this->parentData.incrementCounter();

    while(true) {
        for(const Tunnel &tInfo: this->tunnels) {
            if(tInfo.getDirection() != this->destinationPlace) {
                int tid = tInfo.getTunnelId();
                this->sendReqEnterToDispatchers(tid);
                auto [IDs, res] = this->recvResFromDispatchers(MsgSourceTag::Dispatcher, this->dispatchers);
                if(res.positive == this->dispatchersAmount) {
                    return tid;
                } else if(res.negative) {
                    this->sendReqRollbackToDispatchers(tid, IDs);
                    this->recvResFromDispatchers(MsgSourceTag::Dispatcher, IDs);
                } else {
                    int neutral = res.neutral;
                    //czekaj iles czasu bo sa neutralne, i sprawdz po tym czasie czy wszyscy wyslali Accept, jak nie to anuluj wszystko im
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    this->sendReqStatusToDispatchers(tid);
                    auto [IDs, res] = this->recvResFromDispatchers(MsgSourceTag::RollbackWalker, this->dispatchers);
                    this->clearWalkerChannel(neutral - res.neutral);
                    if(res.positive == this->dispatchersAmount) {
                        return tid;
                    } else {
                        this->sendReqRollbackToDispatchers(tid, IDs);
                        this->recvResFromDispatchers(MsgSourceTag::Dispatcher, IDs);
                    }
                }
            }
        }
    }
}

void TunnelWalker::sendReqStatusToDispatchers(int tunnel_id)
{
    MsgSender walkerSender(this->id, this->dispatchers);
    walkerSender.send(Packet(Sender::Walker, Receiver::Dispatcher, Request::Status,
                             this->parentData.getInfo(), tunnel_id));
}

void TunnelWalker::sendReqEnterToDispatchers(int tunnel_id)
{
    MsgSender walkerSender(this->id, this->dispatchers); 
    walkerSender.send(Packet(Sender::Walker, Receiver::Dispatcher, Request::Enter,
                             this->parentData.getInfo(), tunnel_id));
}

void TunnelWalker::sendReqExitToDispatchers(int tunnel_id)
{
    this->parentData.incrementCounter();
    Packet p(Sender::Walker, Receiver::Dispatcher, Request::Exit,
             this->parentData.getInfo(), tunnel_id);
    MsgSender(this->id, this->dispatchers).send(p);
}

void TunnelWalker::sendReqRollbackToDispatchers(int tunnel_id, DispatchersID dis)
{
    MsgSender walkerSender(this->id, dis);
    walkerSender.send(Packet(Sender::Walker, Receiver::Dispatcher, Request::Exit,
                             this->parentData.getInfo(), tunnel_id));
}

void TunnelWalker::clearWalkerChannel(int neutralDispatchers)
{
    MsgReceiver walkerReceiver(this->id, this->dispatchers);

    for(int i=0; i<neutralDispatchers; i++) {
        walkerReceiver.wait(MsgSourceTag::Dispatcher);
    }
}

std::tuple<TunnelWalker::DispatchersID,TunnelWalker::Responses>
TunnelWalker::recvResFromDispatchers(MsgComm::MsgSourceTag tag, DispatchersID dispatchers)
{
    MsgReceiver walkerReceiver(this->id, dispatchers);
    int positive = 0, neutral = 0, negative = 0;
    std::vector<int> dispatchersRollback;

    do {
        Packet p = walkerReceiver.wait(tag);

        p.getCmd([](Request){}, [&](Response res){
            switch(res) {
                case Response::Accept:
                    dispatchersRollback.push_back(p.getData().getId());
                    positive++;
                break;

                case Response::InQueue:
                    dispatchersRollback.push_back(p.getData().getId());
                    neutral++;
                break;

                case Response::Deny:
                    negative++;
                break;

                default:
                    throw std::runtime_error("cannot handle reply code in walker " + std::string(__FILE__) + " " + std::to_string(__LINE__));
            }
        });
    } while(positive + neutral + negative < int(dispatchers.size()));

    TunnelWalker::Responses r;
    r.positive = positive; r.neutral = neutral; r.negative = negative;
    return std::make_pair(dispatchersRollback, r);
}

void TunnelWalker::print(std::string str)
{
    #ifdef APP_DEBUG_WALKER
    dstream.write("WALKER: " + str);
    #endif
}

void TunnelWalker::exitTunnel(int tunnel_id)
{
    this->sendReqExitToDispatchers(tunnel_id);
    this->recvResFromDispatchers(MsgSourceTag::Dispatcher, this->dispatchers);
}

void TunnelWalker::changePlace()
{
    this->destinationPlace = this->destinationPlace == Place::Earth ? Place::Dimension : Place::Earth;
}

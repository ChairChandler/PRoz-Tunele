#include "msgsender.h"
#include <stdexcept>
#include <mpi.h>

#include "utils/appdebug.h"
#ifdef APP_DEBUG_COMMUNICATION
#include "utils/distributedstream.h"
#endif

MsgSender::MsgSender(int sender_id, const Target &target_id):
    sender_id(sender_id), target_id(target_id)
{

}

MsgSender::MsgSender(int sender_id, const MsgSender::Target &target_id, MsgComm::MsgSourceTag src):
    sender_id(sender_id), target_id(target_id), specialTag(src), isSpecialTag(true)
{

}

void MsgSender::send(const Packet &packet)
{
    this->packetToSend = packet;
    this->handleOperation();
}

void MsgSender::handleOperation()
{
    struct _ {
        MsgSender &p;
        explicit _(MsgSender &p): p(p) {}
        void operator()(int val) {p(val);}
        void operator()(std::vector<int> val) {p(val);}
    };
    std::visit(_(*this), this->target_id);
}

void MsgSender::operator()(int target)
{   
    int tag;
    MsgComm::MsgSourceTag src;
    if(this->isSpecialTag) {
        src = this->specialTag;
    } else if(this->packetToSend.getReceiver() == MsgComm::Receiver::Walker) {
        src = MsgComm::MsgSourceTag::Dispatcher;
    } else {
        src = MsgComm::MsgSourceTag::Walker;
    }

    tag = static_cast<int>(src);

    #ifdef APP_DEBUG_COMMUNICATION
        bool isRequest;
        std::string action;
        this->packetToSend.getCmd(
                    [&action, &isRequest](MsgComm::Request req){action = describe(req); isRequest = true;},
                    [&action, &isRequest](MsgComm::Response res){action = describe(res); isRequest = false;});

        dstream.write("SEND "
                      "PACKET_NO[" + std::to_string(this->packetToSend.getPacketNo()) + "] " +
                      "TAG[" + describe(src) + "] " +
                      "SENDER[" + describe(this->packetToSend.getSender()) + ", " + std::to_string(this->sender_id) + "] " +
                      "RECEIVER[" + describe(this->packetToSend.getReceiver()) + ", " + std::to_string(target) + "] " +
                      "ACTION[" + action + "]");
    #endif

    MPI_Send(&this->packetToSend, sizeof(Packet), MPI_BYTE, target, tag, MPI_COMM_WORLD); // send packet(data)
}

void MsgSender::operator()(std::vector<int> target)
{
    for(int id: target) {
        this->operator()(id);
    }
}

MsgSender::Target MsgSender::getTargetId() const
{
    return target_id;
}

int MsgSender::getSenderId() const
{
    return sender_id;
}

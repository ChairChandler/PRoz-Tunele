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

void MsgSender::send(const Packet &packet)
{
    this->packetToSend = packet;
    std::visit(this, this->target_id);
}

void MsgSender::operator()(int target)
{   
    int tag;
    if(target != this->sender_id) {
        tag = static_cast<int>(MsgComm::MsgSourceTag::Dispatcher); // dispatcher -> dispatcher
    } else if(this->packetToSend.getReceiver() == MsgComm::Receiver::Walker) {
        tag = static_cast<int>(MsgComm::MsgSourceTag::Dispatcher);
    } else {
        tag = static_cast<int>(MsgComm::MsgSourceTag::Walker);
    }

    #ifdef APP_DEBUG_COMMUNICATION
        bool isRequest;
        std::string action;
        this->packetToSend.getCmd(
                    [&action, &isRequest](MsgComm::Request req){action = describe(req); isRequest = true;},
                    [&action, &isRequest](MsgComm::Response res){action = describe(res); isRequest = false;});

        dstream.write("PACKET_NO[" + std::to_string(this->packetToSend.getPacketNo()) + "] " +
                      std::string((isRequest ? "SEND[REQUEST] " : "SEND[RESPONSE] ")) +
                      "SENDER[" + describe(this->packetToSend.getSender()) + ", " + std::to_string(this->sender_id) + "] " +
                      "RECEIVER[" + describe(this->packetToSend.getReceiver()) + ", " + std::to_string(target) + "] " +
                      "ACTION[" + action + "]");
    #endif

    MPI_Send(&this->packetToSend, sizeof(Packet), MPI_BYTE, target, tag, MPI_COMM_WORLD); // send packet(data)

    #ifdef APP_DEBUG_COMMUNICATION
        dstream.write("PACKET_NO[" + std::to_string(this->packetToSend.getPacketNo()) + "] SENT");
    #endif
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

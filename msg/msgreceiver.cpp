#include "msgreceiver.h"
#include <mpi.h>

#include "utils/appdebug.h"
#ifdef APP_DEBUG_COMMUNICATION
#include "utils/distributedstream.h"
#endif

MsgReceiver::MsgReceiver(int receiverId, MsgReceiver::Target targetId):
    target_id(targetId), receiver_id(receiverId)
{

}

Packet MsgReceiver::wait(MsgComm::MsgSourceTag tag)
{
    this->sourceTag = tag;
    this->handleOperation();
    return this->packetToReceive;
}

void MsgReceiver::handleOperation()
{
    struct _ {
        MsgReceiver &p;
        explicit _(MsgReceiver &p): p(p) {}
        void operator()(int val) {p(val);}
        void operator()(std::vector<int> val) {p(val);}
    };
    std::visit(_(*this), this->target_id);
}

void MsgReceiver::operator()(int target)
{
    Packet packet;
    int tag = static_cast<int>(this->sourceTag);

    MPI_Recv(&packet, sizeof(Packet), MPI_BYTE, target, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
#ifdef APP_DEBUG_COMMUNICATION
    bool isRequest;
    std::string action;
    packet.getCmd(
                [&action, &isRequest](MsgComm::Request req){action = describe(req); isRequest = true;},
    [&action, &isRequest](MsgComm::Response res){action = describe(res); isRequest = false;});

    dstream.write("RECEIVED "
                  "PACKET_NO[" + std::to_string(packet.getPacketNo()) + "] " +
                  "TAG[" + describe(this->sourceTag) + "] " +
                  "SENDER[" + describe(packet.getSender()) + ", " + std::to_string(target) + "] " +
                  "RECEIVER[" + describe(packet.getReceiver()) + ", " + std::to_string(this->receiver_id) + "] " +
                  "ACTION[" + action + "]");
#endif
    this->packetToReceive = packet;
}

void MsgReceiver::operator()(std::vector<int> target)
{
    int tag = static_cast<int>(this->sourceTag);

    MPI_Status status;
    Packet packet;
    int flag;
    while(true) {
        for(int id: target) {
            MPI_Iprobe(id, tag, MPI_COMM_WORLD, &flag, &status);
            if(flag) {
                MPI_Recv(&packet, sizeof(Packet), MPI_BYTE, id, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
#ifdef APP_DEBUG_COMMUNICATION
                bool isRequest;
                std::string action;
                packet.getCmd(
                            [&action, &isRequest](MsgComm::Request req){action = describe(req); isRequest = true;},
                [&action, &isRequest](MsgComm::Response res){action = describe(res); isRequest = false;});

                dstream.write("RECEIVED "
                              "PACKET_NO[" + std::to_string(packet.getPacketNo()) + "] " +
                              "TAG[" + describe(this->sourceTag) + "] " +
                              "SENDER[" + describe(packet.getSender()) + ", " + std::to_string(id) + "] " +
                              "RECEIVER[" + describe(packet.getReceiver()) + ", " + std::to_string(this->receiver_id) + "] " +
                              "ACTION[" + action + "]");
#endif
                this->packetToReceive = packet;
                return;
            }
        }
    }
}

MsgReceiver::Target MsgReceiver::getTargetId() const
{
    return target_id;
}

int MsgReceiver::getReceiverId() const
{
    return receiver_id;
}

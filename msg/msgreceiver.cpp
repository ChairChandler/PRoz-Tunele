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

Packet MsgReceiver::wait()
{
    this->sourceTag = MsgComm::MsgSourceTag::Unknown;
    std::visit(this, this->target_id);
    return this->packetToReceive;
}

Packet MsgReceiver::wait(MsgComm::MsgSourceTag tag)
{
    this->sourceTag = tag;
    std::visit(this, this->target_id);
    return this->packetToReceive;
}

void MsgReceiver::operator()(int target)
{
    Packet packet;
    if(this->sourceTag == MsgComm::MsgSourceTag::Unknown) {
        #ifdef APP_DEBUG_COMMUNICATION
            dstream.write("SOURCE[UNKNOWN] RECEIVER[" + std::to_string(this->receiver_id) + "]");
        #endif
        MPI_Recv(&packet, sizeof(Packet), MPI_BYTE, target, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        #ifdef APP_DEBUG_COMMUNICATION
            bool isRequest;
            std::string action;
            packet.getCmd(
                        [&action, &isRequest](MsgComm::Request req){action = describe(req); isRequest = true;},
                        [&action, &isRequest](MsgComm::Response res){action = describe(res); isRequest = false;});

            dstream.write("PACKET_NO[" + std::to_string(packet.getPacketNo()) + "] " +
                          "SOURCE[UNKNOWN] " +
                          std::string((isRequest ? "RECEIVE[REQUEST] " : "RECEIVE[RESPONSE] ")) +
                          "SENDER[" + describe(packet.getSender()) + ", " + std::to_string(target) + "] " +
                          "RECEIVER[" + describe(packet.getReceiver()) + ", " + std::to_string(this->receiver_id) + "] " +
                          "ACTION[" + action + "]");
        #endif
    } else {
        int tag = static_cast<int>(this->sourceTag);

        #ifdef APP_DEBUG_COMMUNICATION
            dstream.write("SOURCE[" + describe(this->sourceTag) + "] RECEIVER[" + std::to_string(this->receiver_id) + "]");
        #endif
        MPI_Recv(&packet, sizeof(Packet), MPI_BYTE, target, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        #ifdef APP_DEBUG_COMMUNICATION
            bool isRequest;
            std::string action;
            packet.getCmd(
                        [&action, &isRequest](MsgComm::Request req){action = describe(req); isRequest = true;},
                        [&action, &isRequest](MsgComm::Response res){action = describe(res); isRequest = false;});

            dstream.write("PACKET_NO[" + std::to_string(packet.getPacketNo()) + "] " +
                          "SOURCE[" + describe(this->sourceTag) + "] " +
                          std::string((isRequest ? "RECEIVE[REQUEST] " : "RECEIVE[RESPONSE] ")) +
                          "SENDER[" + describe(packet.getSender()) + ", " + std::to_string(target) + "] " +
                          "RECEIVER[" + describe(packet.getReceiver()) + ", " + std::to_string(this->receiver_id) + "] " +
                          "ACTION[" + action + "]");
        #endif

    }
    this->packetToReceive = packet;
}

void MsgReceiver::operator()(std::vector<int> target)
{
    auto iter = [this, &target](int tag)
    {
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

                        dstream.write("PACKET_NO[" + std::to_string(packet.getPacketNo()) + "] " +
                                      "SOURCE[" + describe(this->sourceTag) + "] " +
                                      std::string((isRequest ? "RECEIVE[REQUEST] " : "RECEIVE[RESPONSE] ")) +
                                      "SENDER[" + describe(packet.getSender()) + ", " + std::to_string(id) + "] " +
                                      "RECEIVER[" + describe(packet.getReceiver()) + ", " + std::to_string(this->receiver_id) + "] " +
                                      "ACTION[" + action + "]");
                    #endif
                    return packet;
                }
            }
        }
    };

    if(this->sourceTag == MsgComm::MsgSourceTag::Unknown) {
        #ifdef APP_DEBUG_COMMUNICATION
            dstream.write("SOURCE[UNKNOWN] RECEIVER[" + std::to_string(this->receiver_id) + "]");
        #endif
        this->packetToReceive = iter(MPI_ANY_TAG);
    } else {
        int tag = static_cast<int>(this->sourceTag);
        #ifdef APP_DEBUG_COMMUNICATION
            dstream.write("SOURCE[" + describe(this->sourceTag) + "] RECEIVER[" + std::to_string(this->receiver_id) + "]");
        #endif
        this->packetToReceive = iter(tag);
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

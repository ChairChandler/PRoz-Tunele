#include "msgreceiver.h"
#include <mpi.h>

#include "appdebug.h"
#ifdef APP_DEBUG_COMMUNICATION
#include "distributedstream.h"
#endif

MsgReceiver::MsgReceiver(int receiverId, MsgReceiver::Target targetId, const std::string &receiverName):
    targetId(targetId), receiverId(receiverId), receiverName(receiverName)
{

}

void MsgReceiver::setAllTarget(std::vector<int> target)
{
    Visit::setAllTarget(target);
}

Packet MsgReceiver::wait()
{
    Visit v(this->receiverName);
    std::visit(v.overloaded, this->targetId);
    return v.getPacket();
}

Packet MsgReceiver::wait(MsgComm tag)
{
    Visit v(this->receiverId, tag, this->receiverName);
    std::visit(v.overloaded, this->targetId);
    return v.getPacket();
}

MsgReceiver::Visit::Visit(std::string receiverName): anyTag(true), receiverName(receiverName), overloaded(*this)
{

}

MsgReceiver::Visit::Visit(int receiverId, MsgComm tag, std::string receiverName):
    anyTag(false), tag(tag), receiverId(receiverId), receiverName(receiverName), overloaded(*this)
{

}

Packet MsgReceiver::Visit::getPacket() const
{
    return this->packet;
}

void MsgReceiver::Visit::setAllTarget(std::vector<int> target)
{
    Visit::allTarget = target;
}

MsgReceiver::Visit::Overloaded::Overloaded(MsgReceiver::Visit &v): v(v)
{

}

void MsgReceiver::Visit::Overloaded::operator()(int target)
{
    Packet packet;
    if(v.anyTag) {
        #ifdef APP_DEBUG_COMMUNICATION
            dstream.write(v.receiverName, "Waiting for packet from " + std::to_string(target) + " with tag any");
        #endif
        MPI_Recv(&packet, sizeof(Packet), MPI_BYTE, target, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        #ifdef APP_DEBUG_COMMUNICATION
            dstream.write(v.receiverName, "Packet " + describe(packet.type) +
                          " received from " + std::to_string(target) + " with tag any");
        #endif
    } else {
        int tag = static_cast<int>(v.tag);
        #ifdef APP_DEBUG_COMMUNICATION
            dstream.write(v.receiverName, "Waiting for packet from " + std::to_string(target) + " with tag " + describe(v.tag));
        #endif
        MPI_Recv(&packet, sizeof(Packet), MPI_BYTE, target, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        #ifdef APP_DEBUG_COMMUNICATION
            dstream.write(v.receiverName, "Packet " + describe(packet.type) +
                          " received from " + std::to_string(target) + " with tag " + describe(v.tag));
        #endif
    }
    v.packet = packet;
}

void MsgReceiver::Visit::Overloaded::operator()(std::vector<int> target)
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
                        dstream.write(v.receiverName, "Received packet " + describe(packet.type) + " from " + std::to_string(id) + " with tag " +
                                      (tag > -1 ? describe(v.tag) : "any"));
                    #endif
                    return packet;
                }
            }
        }
    };

    if(v.anyTag) {
    #ifdef APP_DEBUG_COMMUNICATION
        dstream.write(v.receiverName, "Waiting for packet from any with tag any");
    #endif
        v.packet = iter(MPI_ANY_TAG);
    } else {
        int tag = static_cast<int>(v.tag);
    #ifdef APP_DEBUG_COMMUNICATION
        dstream.write(v.receiverName, "Waiting for packet from any with tag " + describe(v.tag));
    #endif
        v.packet = iter(tag);
    }
}

void MsgReceiver::Visit::Overloaded::operator()(SpecificTarget target)
{
    switch(target) {
        case SpecificTarget::Self:
            this->operator()(v.receiverId);
        break;

        case SpecificTarget::All:
            this->operator()(v.allTarget);
        break;
    }
}

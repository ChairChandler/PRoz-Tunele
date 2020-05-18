#include "msgreceiver.h"
#include <mpi.h>

MsgReceiver::MsgReceiver(int receiverId, MsgReceiver::Target targetId):
    targetId(targetId), receiverId(receiverId)
{

}

void MsgReceiver::setAllTarget(std::vector<int> target)
{
    Visit::setAllTarget(target);
}

Packet MsgReceiver::wait()
{
    Visit v;
    std::visit(v.overloaded, this->targetId);
    return v.getPacket();
}

Packet MsgReceiver::wait(MsgComm tag)
{
    Visit v(this->receiverId, tag);
    std::visit(v.overloaded, this->targetId);
    return v.getPacket();
}

MsgReceiver::Visit::Visit(): anyTag(true), overloaded(*this)
{

}

MsgReceiver::Visit::Visit(int receiverId, MsgComm tag):
    anyTag(false), tag(tag), receiverId(receiverId), overloaded(*this)
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
    int size;
    if(v.anyTag) {
        MPI_Status status;
        MPI_Recv(&size, 1, MPI_INT, target, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&packet, size, MPI_BYTE, target, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
        int tag = static_cast<int>(v.tag);
        MPI_Recv(&size, 1, MPI_INT, target, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&packet, size, MPI_BYTE, target, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    v.packet = packet;
}

void MsgReceiver::Visit::Overloaded::operator()(std::vector<int> target)
{
    auto iter = [&target](int tag)
    {
        MPI_Request request;
        MPI_Status status;
        Packet packet;
        int size, flag;
        while(true) {
            for(int id: target) {
                MPI_Irecv(&size, 1, MPI_INT, id, tag, MPI_COMM_WORLD, &request);
                MPI_Test(&request, &flag, &status);
                if(flag) {
                    MPI_Recv(&packet, size, MPI_BYTE, id, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    return packet;
                }
            }
        }
    };

    if(v.anyTag) {
        v.packet = iter(MPI_ANY_TAG);
    } else {
        int tag = static_cast<int>(v.tag);
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

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
    std::visit(v, this->targetId);
    return v.getPacket();
}

Packet MsgReceiver::wait(MsgComm tag)
{
    Visit v(this->receiverId, tag);
    std::visit(v, this->targetId);
    return v.getPacket();
}

MsgReceiver::Visit::Visit(): anyTag(true)
{

}

MsgReceiver::Visit::Visit(int receiverId, MsgComm tag):
    anyTag(false), tag(tag), receiverId(receiverId)
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

void MsgReceiver::Visit::operator()(int target)
{
    Packet packet;
    int size;
    if(anyTag) {
        MPI_Status status;
        MPI_Recv(&size, 1, MPI_INT, target, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Recv(&packet, size, MPI_BYTE, target, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    } else {
        int tag = static_cast<int>(this->tag);
        MPI_Recv(&size, 1, MPI_INT, target, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&packet, size, MPI_BYTE, target, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    this->packet = packet;
}

void MsgReceiver::Visit::operator()(std::vector<int> target)
{
    auto iter = [&target](int tag)
    {
        Packet packet;
        int size;
        for(int id: target) {
            MPI_Request request;
            MPI_Irecv(&size, 1, MPI_INT, id, tag, MPI_COMM_WORLD, &request);
            MPI_Status status;
            MPI_Request_get_status(request, nullptr, &status);
            MPI_Recv(&packet, size, MPI_BYTE, id, status.MPI_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        return packet;
    };

    if(anyTag) {
        this->packet = iter(MPI_ANY_TAG);
    } else {
        int tag = static_cast<int>(this->tag);
        this->packet = iter(tag);
    }
}

void MsgReceiver::Visit::operator()(SpecificTarget target)
{
    switch(target) {
        case SpecificTarget::Self:
            this->operator()(this->receiverId);
        break;

        case SpecificTarget::All:
            this->operator()(this->allTarget);
        break;
    }
}

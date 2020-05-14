#include "msgsender.h"
#include <stdexcept>
#include <mpi.h>

MsgSender::MsgSender(Target targetId): targetId(targetId)
{

}

void MsgSender::setAllTarget(std::vector<int> target)
{
    Visit::setAllTarget(target);
}

void MsgSender::sendRequest(Request msg, RichmanInfo payload, Tunnel tunnel)
{
    Packet p(msg, payload, tunnel);
    std::visit(Visit(MsgComm::RequestRecvTag, p), this->targetId);
}

void MsgSender::sendReply(Reply msg, RichmanInfo payload, Tunnel tunnel)
{
    Packet p(msg, payload, tunnel);
    std::visit(Visit(MsgComm::ReplyRecvTag, p), this->targetId);
}

MsgSender::Visit::Visit(MsgComm tag, Packet packet): tag(tag), packet(packet)
{

}

void MsgSender::Visit::setAllTarget(std::vector<int> target)
{
    Visit::allTarget = target;
}

void MsgSender::Visit::operator()(int target)
{
    int size = sizeof(this->packet);
    int tag = static_cast<int>(this->tag);
    MPI_Send(&size, 1, MPI_INT, target, tag, MPI_COMM_WORLD); // first send size
    MPI_Send(&this->packet, size, MPI_BYTE, target, tag, MPI_COMM_WORLD); // then send packet(data)
}

void MsgSender::Visit::operator()(std::vector<int> target)
{
    for(int id: target) {
        this->operator()(id);
    }
}

void MsgSender::Visit::operator()(SpecificTarget target)
{
    switch(target) {
        case SpecificTarget::All:
            if(this->allTarget.size()) {
                this->operator()(allTarget);
            } else {
                throw std::runtime_error("empty targets");
            }
        break;

        case SpecificTarget::Self:
            this->operator()(this->packet.data.id);
        break;
    }
}

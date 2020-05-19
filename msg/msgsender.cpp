#include "msgsender.h"
#include <stdexcept>
#include <mpi.h>

MsgSender::MsgSender(Target targetId): isSenderIdInit(false), targetId(targetId), senderId()
{

}

MsgSender::MsgSender(int senderId, MsgSender::Target targetId): isSenderIdInit(true), targetId(targetId), senderId(senderId)
{

}

void MsgSender::setAllTarget(std::vector<int> target)
{
    Visit::setAllTarget(target);
}

void MsgSender::sendRequest(Request msg, RichmanInfo payload, int tunnel_id)
{
    Packet p(msg, payload, tunnel_id);
    std::visit(Visit(MsgComm::RequestRecvTag, p).overloaded, this->targetId);
}

void MsgSender::sendReply(Reply msg, RichmanInfo payload, int tunnel_id)
{
    Packet p(msg, payload, tunnel_id);
    std::visit(Visit(MsgComm::ReplyRecvTag, p).overloaded, this->targetId);
}

MsgSender::Visit::Visit(MsgComm tag, Packet packet):
    isSenderIdInit(false), senderId(), tag(tag), packet(packet), overloaded(*this)
{

}

MsgSender::Visit::Visit(MsgComm tag, Packet packet, int senderId):
    isSenderIdInit(true), senderId(senderId), tag(tag), packet(packet), overloaded(*this)
{

}

void MsgSender::Visit::setAllTarget(std::vector<int> target)
{
    Visit::allTarget = target;
}

MsgSender::Visit::Overloaded::Overloaded(MsgSender::Visit &v): v(v)
{

}

void MsgSender::Visit::Overloaded::operator()(int target)
{
    if(!v.isSenderIdInit || target != v.senderId) { // later it should be changed to decorator
        int tag = static_cast<int>(v.tag);
        MPI_Send(&v.packet, sizeof(Packet), MPI_BYTE, target, tag, MPI_COMM_WORLD); // send packet(data)
    }
}

void MsgSender::Visit::Overloaded::operator()(std::vector<int> target)
{
    for(int id: target) {
        this->operator()(id);
    }
}

void MsgSender::Visit::Overloaded::operator()(SpecificTarget target)
{
    switch(target) {
        case SpecificTarget::All:
            if(v.allTarget.size()) {
                this->operator()(allTarget);
            } else {
                throw std::runtime_error("empty targets");
            }
        break;

        case SpecificTarget::Self:
            this->operator()(v.packet.data.getId());
        break;
    }
}

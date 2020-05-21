#include "msgsender.h"
#include <stdexcept>
#include <mpi.h>

#include "appdebug.h"
#ifdef APP_DEBUG_COMMUNICATION
#include "distributedstream.h"
#endif

MsgSender::MsgSender(Target targetId, const std::string &senderName):
    isSenderIdInit(false), targetId(targetId), senderId(), senderName(senderName)
{

}

MsgSender::MsgSender(int senderId, MsgSender::Target targetId, const std::string &senderName):
    isSenderIdInit(true), targetId(targetId), senderId(senderId), senderName(senderName)
{

}

void MsgSender::setAllTarget(std::vector<int> target)
{
    Visit::setAllTarget(target);
}

void MsgSender::sendRequest(Request msg, RichmanInfo payload, int tunnel_id)
{
    Packet p(msg, payload, tunnel_id);
    std::visit(Visit(MsgComm::RequestRecvTag, p, this->senderName).overloaded, this->targetId);
}

void MsgSender::sendReply(Reply msg, RichmanInfo payload, int tunnel_id)
{
    Packet p(msg, payload, tunnel_id);
    std::visit(Visit(MsgComm::ReplyRecvTag, p, this->senderName).overloaded, this->targetId);
}

MsgSender::Visit::Visit(MsgComm tag, Packet packet, const std::string &senderName):
    isSenderIdInit(false), senderId(), tag(tag), packet(packet), senderName(senderName), overloaded(*this)
{

}

MsgSender::Visit::Visit(MsgComm tag, Packet packet, int senderId, const std::string &senderName):
    isSenderIdInit(true), senderId(senderId), tag(tag), packet(packet), senderName(senderName), overloaded(*this)
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
        #ifdef APP_DEBUG_COMMUNICATION
            dstream.write(v.senderName, "Send packet " + describe(v.packet.type) +
                          " to " + std::to_string(target) + " with tag " + describe(v.tag));
        #endif
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
            }
        break;

        case SpecificTarget::Self:
            this->operator()(v.packet.data.getId());
        break;
    }
}

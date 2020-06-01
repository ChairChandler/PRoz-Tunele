#include "packet.h"

Packet::Packet(MsgComm::Sender sender, MsgComm::Receiver receiver, MsgComm::Request cmd, RichmanInfo data, int tunnel_id):
    sender(sender), receiver(receiver), cmd(cmd), data(data), tunnel_id(tunnel_id)
{
    this->incrementPacketNo();
}

Packet::Packet(MsgComm::Sender sender, MsgComm::Receiver receiver, MsgComm::Response cmd, RichmanInfo data, int tunnel_id):
    sender(sender), receiver(receiver), cmd(cmd), data(data), tunnel_id(tunnel_id)
{
    this->incrementPacketNo();
}

void Packet::incrementPacketNo()
{
    thread_local unsigned int packetCounter = 0;
    this->packetNo = packetCounter++;
}

MsgComm::Receiver Packet::getReceiver() const
{
    return receiver;
}

void Packet::getCmd(Packet::UnpackRequest req, Packet::UnpackResponse res) const
{
    using namespace MsgComm;
    struct _ {
        UnpackRequest a;
        UnpackResponse b;
        explicit _(UnpackRequest a, UnpackResponse b): a(a), b(b) {}
        void operator()(Request r) {a(r);};
        void operator()(Response r) {b(r);};
    } v(req, res);
    std::visit(v, this->cmd);
}

RichmanInfo Packet::getData() const
{
    return data;
}

int Packet::getTunnel_id() const
{
    return tunnel_id;
}

MsgComm::Sender Packet::getSender() const
{
    return sender;
}

unsigned int Packet::getPacketNo() const
{
    return packetNo;
}

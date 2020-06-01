#ifndef PACKET_H
#define PACKET_H
#include <variant>
#include <functional>
#include "msgcomm.h"
#include "richmaninfo.h"

class Packet
{
    unsigned int packetNo;
    MsgComm::Sender sender;
    MsgComm::Receiver receiver;
    std::variant<MsgComm::Request, MsgComm::Response> cmd;
    RichmanInfo data;
    int tunnel_id;

    void incrementPacketNo();
public:
    using UnpackRequest = std::function<void(MsgComm::Request)>;
    using UnpackResponse = std::function<void(MsgComm::Response)>;

    Packet() = default;
    Packet(MsgComm::Sender sender, MsgComm::Receiver receiver, MsgComm::Request cmd, RichmanInfo data, int tunnel_id); // ready request
    Packet(MsgComm::Sender sender, MsgComm::Receiver receiver, MsgComm::Response cmd, RichmanInfo data, int tunnel_id); // ready response

    MsgComm::Sender getSender() const;
    MsgComm::Receiver getReceiver() const;
    void getCmd(UnpackRequest req, UnpackResponse res) const;
    RichmanInfo getData() const;
    int getTunnel_id() const;
    unsigned int getPacketNo() const;
};

#endif // PACKET_H

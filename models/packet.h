#ifndef PACKET_H
#define PACKET_H
#include <variant>
#include "request.h"
#include "reply.h"
#include "richmaninfo.h"
#include "tunnel/tunnel.h"

struct Packet
{
    std::variant<Request, Reply> type;
    RichmanInfo data;
    int tunnel_id;

    // empty packet
    Packet(): data(-1)
    {

    }
    // ready packet
    Packet(std::variant<Request, Reply> type, RichmanInfo data, int tunnel_id):
        type(type), data(data), tunnel_id(tunnel_id)
    {

    }
};

#endif // PACKET_H

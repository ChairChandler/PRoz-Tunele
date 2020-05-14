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
    Tunnel tunnel;

    // empty packet
    Packet(): data(-1), tunnel(-1, -1, -1, Place::Earth)
    {

    }
    // ready packet
    Packet(std::variant<Request, Reply> type, RichmanInfo data, Tunnel tunnel):
        type(type), data(data), tunnel(tunnel)
    {

    }
};

#endif // PACKET_H

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

inline std::string describe(const std::variant<Request, Reply> &v)
{
    std::string d;
    struct _ {
       std::string &d;
       _(std::string&d): d(d) {}
       void operator()(Request r) {d = describe(r);}
       void operator()(Reply r) {d = describe(r);}
    };

    std::visit(_(d), v);
    return d;
}

#endif // PACKET_H

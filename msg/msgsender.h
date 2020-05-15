#ifndef MSGSENDER_H
#define MSGSENDER_H
#include "msgcomm.h"
#include "models/request.h"
#include "models/reply.h"
#include "models/richmaninfo.h"
#include "tunnel/tunnel.h"
#include "models/packet.h"
#include <variant>
#include <vector>

class MsgSender
{
public:
    using Target = std::variant<int, std::vector<int>, SpecificTarget>;
private:
    class Visit // handle variant different types
    {
        const MsgComm tag;
        const Packet packet;
        static std::vector<int> allTarget;
    public:
        Visit(MsgComm tag, Packet packet);
        static void setAllTarget(std::vector<int> target);
        void operator()(int target);
        void operator()(std::vector<int> target);
        void operator()(SpecificTarget target);
    };
    const Target targetId;
    void send(Packet packet, int dest);
public:
    explicit MsgSender(Target targetId);
    static void setAllTarget(std::vector<int> target);
    void sendRequest(Request msg, RichmanInfo payload, int tunnel_id);
    void sendReply(Reply msg, RichmanInfo payload, int tunnel_id);
};

#endif // MSGSENDER_H

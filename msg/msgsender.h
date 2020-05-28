#ifndef MSGSENDER_H
#define MSGSENDER_H
#include "models/msgcomm.h"
#include "models/richmaninfo.h"
#include "tunnel/tunnel.h"
#include "models/packet.h"
#include <variant>
#include <vector>

class MsgSender
{
public:
    using Target = std::variant<int, std::vector<int>>;
private:
    const int sender_id;
    const Target target_id;

    Packet packetToSend;

    void operator()(int target);
    void operator()(std::vector<int> target);
public:
    explicit MsgSender(int sender_id, const Target &target_id);

    void send(const Packet &packet);

    Target getTargetId() const;
    int getSenderId() const;
};

#endif // MSGSENDER_H

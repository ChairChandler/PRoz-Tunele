#ifndef MSGRECEIVER_H
#define MSGRECEIVER_H
#include "models/packet.h"
#include "models/msgcomm.h"
#include <vector>

class MsgReceiver
{
public:
    using Target = std::variant<int, std::vector<int>>;
private:
    const Target target_id;
    const int receiver_id;

    MsgComm::MsgSourceTag sourceTag;
    Packet packetToReceive;

    void handleOperation();
    void operator()(int target);
    void operator()(std::vector<int> target);
public:
    explicit MsgReceiver(int receiver_id, Target target_id);

    Packet wait(MsgComm::MsgSourceTag tag);

    Target getTargetId() const;
    int getReceiverId() const;
};

#endif // MSGRECEIVER_H

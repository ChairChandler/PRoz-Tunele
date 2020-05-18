#ifndef MSGRECEIVER_H
#define MSGRECEIVER_H
#include "models/packet.h"
#include "msgcomm.h"
#include <vector>

class MsgReceiver
{
public:
    using Target = std::variant<int, std::vector<int>, SpecificTarget>;
private:
    class Visit // handle variant different types
    {
        bool anyTag;
        MsgComm tag;
        Packet packet;
        int receiverId;
        static inline std::vector<int> allTarget = std::vector<int>();
    public:
        explicit Visit();
        explicit Visit(int receiverId, MsgComm tag);
        Packet getPacket() const;
        static void setAllTarget(std::vector<int> target);

        struct Overloaded {
            Visit &v;
            explicit Overloaded(Visit &);
            void operator()(int target);
            void operator()(std::vector<int> target);
            void operator()(SpecificTarget target);
        } overloaded;
    };
    const Target targetId;
    const int receiverId;
public:
    explicit MsgReceiver(int receiverId, Target targetId);
    static void setAllTarget(std::vector<int> target);
    Packet wait();
    Packet wait(MsgComm tag);
};

#endif // MSGRECEIVER_H

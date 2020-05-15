#ifndef MSGDISPATCHER_H
#define MSGDISPATCHER_H
#include "runnable.h"
#include "models/richmaninfo.h"
#include "richman.h"
#include "tunnel/tunnel.h"
#include "models/packet.h"
#include <atomic>

class MsgDispatcher: public Runnable
{
    std::atomic<RichmanInfo> &parentData;
    Tunnel earthTunnel, dimTunnel;
    const int richmansAmount;

    struct Visit
    {
        void operator()();
    };

    void handleMsg(Request type);
    void handleMsg(Reply type);
public:
    explicit MsgDispatcher(std::atomic<RichmanInfo> &parentData, size_t queueMaxSize, size_t tunnelMaxSize, int richmansAmount);
    void run() override;
    void executeOperation(Packet packet);
    void handleSelfWalker();
};

#endif // MSGDISPATCHER_H

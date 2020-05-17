#ifndef MSGDISPATCHER_H
#define MSGDISPATCHER_H
#include "runnable.h"
#include "models/richmaninfo.h"
#include "tunnel/tunnel.h"
#include "models/packet.h"
#include <atomic>
#include <map>
#include <memory>

class MsgDispatcher: public Runnable
{
    std::atomic<RichmanInfo> &parentData;
    std::map<int, std::shared_ptr<Tunnel>> tunnels;
    const int richmansAmount;

    int selfWalkerTunnelId;
    int selfWalkerPositiveResponse = 0;
    int selfWalkerNegativeResponse = 0;

    void handleMsg(Request type, RichmanInfo data, int tunnel_id);
    void handleMsg(Reply type, RichmanInfo data);
    void executeOperation(Packet packet);
    void handleSelfWalker();
public:
    using TunnelPtr = std::shared_ptr<Tunnel>;
    using TunnelMap = std::map<int, TunnelPtr>;
    explicit MsgDispatcher(std::atomic<RichmanInfo> &parentData, const TunnelMap &tunnels, int richmansAmount);
    void run() override;
};

#endif // MSGDISPATCHER_H

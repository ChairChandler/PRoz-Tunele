#ifndef MSGDISPATCHER_H
#define MSGDISPATCHER_H
#define UNDEFINED -1
#include "thread/runnable.h"
#include "models/richmaninfo.h"
#include "tunnel/tunnel.h"
#include "models/packet.h"
#include <map>
#include <memory>
#include "atomicrichmaninfo.h"

class MsgDispatcher: public Runnable
{
public:
    using TunnelPtr = std::shared_ptr<Tunnel>;
    using TunnelMap = std::map<int, TunnelPtr>;
private:
    AtomicRichmanInfo &parentData;
    TunnelMap tunnels;
    const int richmansAmount;
    std::vector<int> allTargets, otherDispatchers;
    const int id;

    bool selfWalkerEnterRequest;
    RichmanInfo selfWalkerRichmanInfo;
    int selfWalkerTunnelId = UNDEFINED;
    int selfWalkerPositiveResponse = 0;
    int selfWalkerNegativeResponse = 0;

    void handleRequest(const Packet &packet, MsgComm::Request req);
    void handleResponse(const Packet &packet, MsgComm::Response res);
    void executeOperation(const Packet &packet);
    void handleSelfWalker();
public:
    explicit MsgDispatcher(AtomicRichmanInfo &parentData, const TunnelMap &tunnels, int richmansAmount);
    void run() override;
};

#endif // MSGDISPATCHER_H

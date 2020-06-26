#ifndef MSGDISPATCHER_H
#define MSGDISPATCHER_H
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
    std::vector<int> walkers;
    const int id;

    void executeOperation(const Packet &packet);
    void handleRequest(const Packet &packet, MsgComm::Request req);
    void printTunnels();
    void print(std::string str);
public:
    explicit MsgDispatcher(AtomicRichmanInfo &parentData, const TunnelMap &tunnels, int richmansAmount);
    void run() override;
};

#endif // MSGDISPATCHER_H

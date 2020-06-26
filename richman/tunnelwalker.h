#ifndef TUNNELWALKER_H
#define TUNNELWALKER_H
#include "thread/runnable.h"
#include "models/place.h"
#include "models/richmaninfo.h"
#include "tunnel/tunnel.h"
#include "atomicrichmaninfo.h"
#include <vector>
#include "models/packet.h"

class TunnelWalker: public Runnable
{
    using DispatchersID = std::vector<int>;

    AtomicRichmanInfo &parentData;
    Place destinationPlace;
    std::vector<Tunnel> tunnels;
    const int id, dispatchersAmount;
    DispatchersID dispatchers;

    int enterTunnel();
    void exitTunnel(int tunnel_id);
    void changePlace();

    void sendReqStatusToDispatchers(int tunnel_id);
    void sendReqEnterToDispatchers(int tunnel_id);
    void sendReqExitToDispatchers(int tunnel_id);
    void sendReqRollbackToDispatchers(int tunnel_id, DispatchersID dis);
    void clearWalkerChannel(int neutralDispatchers);

    struct Responses {int positive, neutral, negative;};
    std::tuple<DispatchersID, Responses> recvResFromDispatchers(MsgComm::MsgSourceTag tag, DispatchersID dispatchers);

    void print(std::string str);
public:
    explicit TunnelWalker(AtomicRichmanInfo &parentData, Place startingPlace, const std::vector<Tunnel> &tunnels, int richmansAmount);
    void run() override;
};

#endif // TUNNELWALKER_H

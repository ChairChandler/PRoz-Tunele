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
    AtomicRichmanInfo &parentData;
    Place destinationPlace;
    std::vector<Tunnel> tunnels;
    const int id;

    int enterTunnel();
    bool isEnterResponse(Packet p);
    void exitTunnel(int tunnel_id);
    void changePlace();

    void sendReqToDispatcher(int tunnel_id);
    Packet recvResFromDispatcher();
public:
    explicit TunnelWalker(AtomicRichmanInfo &parentData, Place startingPlace, const std::vector<Tunnel> &tunnels);
    void run() override;
};

#endif // TUNNELWALKER_H

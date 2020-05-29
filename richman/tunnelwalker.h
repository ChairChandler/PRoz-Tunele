#ifndef TUNNELWALKER_H
#define TUNNELWALKER_H
#include "thread/runnable.h"
#include "models/place.h"
#include "models/richmaninfo.h"
#include "tunnel/tunnel.h"
#include "atomicrichmaninfo.h"
#include <chrono>
#include <vector>

class TunnelWalker: public Runnable
{
    using miliseconds = std::chrono::milliseconds;
    const miliseconds waitTime = miliseconds(500);
    AtomicRichmanInfo &parentData;
    Place place;
    std::vector<Tunnel> tInfo;
    const int id;


    int enterTunnel();
    void wait();
    void exitTunnel(int tunnel_id);
public:
    explicit TunnelWalker(AtomicRichmanInfo &parentData, Place startingPlace, const std::vector<Tunnel> &tInfo);
    void run() override;
};

#endif // TUNNELWALKER_H

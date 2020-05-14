#ifndef TUNNELWALKER_H
#define TUNNELWALKER_H
#include "runnable.h"
#include "models/place.h"
#include "models/richmaninfo.h"
#include "tunnel/tunnel.h"
#include <atomic>
#include <chrono>

class TunnelWalker: public Runnable
{
    using miliseconds = std::chrono::milliseconds;
    const miliseconds waitTime = miliseconds(500);
    std::atomic<RichmanInfo> &parentData;
    Place place;
public:
    TunnelWalker(std::atomic<RichmanInfo> &parentData, Place startingPlace);
    void run() override;
    Tunnel enterTunnel();
    void wait();
    void exitTunnel(Tunnel &tunnel);
};

#endif // TUNNELWALKER_H

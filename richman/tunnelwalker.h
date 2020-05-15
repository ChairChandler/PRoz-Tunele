#ifndef TUNNELWALKER_H
#define TUNNELWALKER_H
#include "runnable.h"
#include "models/place.h"
#include "models/richmaninfo.h"
#include "richman.h"
#include "tunnel/tunnel.h"
#include <atomic>
#include <chrono>

class TunnelWalker: public Runnable
{
    using miliseconds = std::chrono::milliseconds;
    const miliseconds waitTime = miliseconds(500);
    std::atomic<RichmanInfo> &parentData;
    Place place;
    const int tunnelsAmount;
public:
    explicit TunnelWalker(std::atomic<RichmanInfo> &parentData, Place startingPlace, int tunnelsAmount);
    void run() override;
    int enterTunnel();
    void wait();
    void exitTunnel(int tunnel_id);
};

#endif // TUNNELWALKER_H

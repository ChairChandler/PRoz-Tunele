#ifndef TUNNELWALKER_H
#define TUNNELWALKER_H
#include "thread/runnable.h"
#include "models/place.h"
#include "models/richmaninfo.h"
#include "tunnel/tunnel.h"
#include <atomic>
#include <chrono>
#include <vector>

class TunnelWalker: public Runnable
{
    using miliseconds = std::chrono::milliseconds;
    const miliseconds waitTime = miliseconds(500);
    std::atomic<RichmanInfo> *parentData;
    Place place;
    std::vector<Tunnel> tInfo;

    const std::string name = "W";

    int enterTunnel();
    void wait();
    void exitTunnel(int tunnel_id);

    void writeStream(const std::string &m);
public:
    explicit TunnelWalker(std::atomic<RichmanInfo> &parentData, Place startingPlace, const std::vector<Tunnel> &tInfo);
    void run() override;
};

#endif // TUNNELWALKER_H

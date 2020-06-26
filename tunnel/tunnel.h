#ifndef TUNNEL_H
#define TUNNEL_H
#include "limiteddeque.h"
#include "limitedset.h"
#include "models/place.h"
#include "models/richmaninfo.h"

class Tunnel
{
    LimitedDeque<RichmanInfo> queue;
    LimitedSet<int> inside;
    Place direction;
    int id;
public:
    Tunnel(int tunnelId, size_t queueMaxSize, size_t insideMaxSize, Place direction);
    int getTunnelId() const;
    Place getDirection() const;

    bool isQueueFilled() const;
    bool isTunnelFilled() const;

    bool isInsideQueue(int id) const;
    bool isInsideTunnel(int id) const;

    Tunnel& appendQueue(const RichmanInfo &info);
    Tunnel& insertTunnel(int id);

    Tunnel& removeFromQueue(int id);
    Tunnel& removeFromTunnel(int id);

    void sortQueue();
    bool isFirstInQueue(const RichmanInfo &info) const;
    std::pair<RichmanInfo, bool> getFromQueue(int pos) const;
    LimitedDeque<RichmanInfo> getQueue() const;
    LimitedSet<int> getInside() const;
};

#endif // TUNNEL_H

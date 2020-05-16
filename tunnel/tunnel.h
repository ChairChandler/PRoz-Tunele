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
    bool isQueueFilled() const;
    bool isInsideFilled() const;
    Tunnel& appendQueue(const RichmanInfo &info);
    Tunnel& insertInside(int id);
    Tunnel& removeFromQueue(const RichmanInfo &info);
    Tunnel& removeFromInside(int id);
    void sortQueueByTime();
    bool isFirstInQueue(const RichmanInfo &info) const;
    std::pair<RichmanInfo, bool> getFromQueue(int pos) const;
};

#endif // TUNNEL_H

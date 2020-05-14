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
};

#endif // TUNNEL_H

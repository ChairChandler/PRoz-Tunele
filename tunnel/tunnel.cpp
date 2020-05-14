#include "tunnel.h"

Tunnel::Tunnel(int tunnelId, size_t queueMaxSize, size_t insideMaxSize, Place direction):
    queue(queueMaxSize), inside(insideMaxSize), direction(direction), id(tunnelId) {

}

int Tunnel::getTunnelId() const
{
    return this->id;
}

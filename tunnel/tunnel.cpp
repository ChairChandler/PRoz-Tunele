#include "tunnel.h"
#include <algorithm>

Tunnel::Tunnel(int tunnelId, size_t queueMaxSize, size_t insideMaxSize, Place direction):
    queue(queueMaxSize), inside(insideMaxSize), direction(direction), id(tunnelId) {

}

int Tunnel::getTunnelId() const
{
    return this->id;
}

bool Tunnel::isQueueFilled() const
{
    return this->queue.size() < this->queue.max_size();
}

Tunnel& Tunnel::appendQueue(const RichmanInfo &info)
{
    this->queue.push_back(info);
    return *this;
}

void Tunnel::sortQueueByTime()
{
    std::sort(this->queue.begin(), this->queue.end(),
              [](const RichmanInfo &a, const RichmanInfo &b){return a.getCounter() < b.getCounter();});
}

bool Tunnel::isFirstInQueue(const RichmanInfo &info) const
{
    return this->queue[0] == info;
}

std::pair<RichmanInfo, bool> Tunnel::getFromQueue(int pos) const
{
    using retType = std::pair<RichmanInfo, bool>;
    try {
        return retType(this->queue.at(pos), true);
    } catch (std::out_of_range) {
        return retType(RichmanInfo(), false);
    }
}

bool Tunnel::isInsideFilled() const
{
    return this->inside.size() < this->inside.max_size();
}

Tunnel &Tunnel::insertInside(int id)
{
    this->inside.insert(id);
    return *this;
}

Tunnel &Tunnel::removeFromQueue(const RichmanInfo &info)
{
    auto it = std::find(this->queue.begin(), this->queue.end(), [&info](const RichmanInfo &a){return info == a;});
    this->queue.erase(it);
    return *this;
}

Tunnel &Tunnel::removeFromInside(int id)
{
    this->inside.erase(id);
    return *this;
}

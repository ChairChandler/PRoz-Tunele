#include "tunnel.h"
#include <algorithm>

Place Tunnel::getDirection() const
{
    return direction;
}

LimitedDeque<RichmanInfo> Tunnel::getQueue() const
{
    return queue;
}

LimitedSet<int> Tunnel::getInside() const
{
    return inside;
}

Tunnel::Tunnel(int tunnelId, size_t queueMaxSize, size_t insideMaxSize, Place direction):
    queue(queueMaxSize), inside(insideMaxSize), direction(direction), id(tunnelId) {

}

int Tunnel::getTunnelId() const
{
    return this->id;
}

bool Tunnel::isQueueFilled() const
{
    return this->queue.size() >= this->queue.max_size();
}

Tunnel& Tunnel::appendQueue(const RichmanInfo &info)
{
    this->queue.push_back(info);
    return *this;
}

void Tunnel::sortQueue() // sort by time and id
{
    std::sort(this->queue.begin(), this->queue.end(),
              [](const RichmanInfo &a, const RichmanInfo &b)
    {
        if(a.getCounter() < b.getCounter()) {
            return true;
        } else if(a.getCounter() > b.getCounter()) {
            return false;
        } else {
            return a.getId() < b.getId();
        }
    });
}

bool Tunnel::isFirstInQueue(const RichmanInfo &info) const
{
    try {
        auto val = this->queue.at(0) == info;
        return val;
    } catch(std::exception) {
        return false;
    }
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

bool Tunnel::isTunnelFilled() const
{
    return this->inside.size() >= this->inside.max_size();
}

bool Tunnel::isInsideQueue(int id) const
{
    auto itr = std::find_if(this->queue.begin(), this->queue.end(),
                        [&id](const RichmanInfo &r){return r.getId() == id;});

    return itr == this->queue.end() ? false : true;
}

bool Tunnel::isInsideTunnel(int id) const
{
    return this->inside.find(id) == this->inside.end() ? false : true;
}

Tunnel &Tunnel::insertTunnel(int id)
{
    this->inside.insert(id);
    return *this;
}

Tunnel &Tunnel::removeFromQueue(int id)
{
    auto it = std::find_if(this->queue.begin(), this->queue.end(), [&id](const RichmanInfo &a){return a.getId() == id;});
    if(it != this->queue.end()) {
        this->queue.erase(it);
    }
    return *this;
}

Tunnel &Tunnel::removeFromTunnel(int id)
{
    auto itr = this->inside.find(id);
    if(itr != this->inside.end()) {
        this->inside.erase(itr);
    }
    return *this;
}

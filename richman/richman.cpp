#include "richman.h"
#include <memory>

Richman::Richman(int id, int richmansAmount, size_t queueCapacity, size_t tunnelCapacity):
    info(RichmanInfo(id)),
    earthTunnel(0, queueCapacity, tunnelCapacity, Place::Earth),
    dimTunnel(1, queueCapacity, tunnelCapacity, Place::Dimension),
    walker(info, Place::Earth, std::vector<Tunnel>{earthTunnel, dimTunnel}),
    dispatcher(info, MsgDispatcher::TunnelMap{
        {earthTunnel.getTunnelId(), std::make_shared<Tunnel>(earthTunnel)},
        {dimTunnel.getTunnelId(), std::make_shared<Tunnel>(dimTunnel)}},
               richmansAmount),
    thWalker(&walker),
    thDispatcher(&dispatcher)
{

}

void Richman::start()
{
    this->thWalker.start();
    this->thDispatcher.start();
}

void Richman::stop()
{
    this->thWalker.stop();
    this->thDispatcher.stop();
}

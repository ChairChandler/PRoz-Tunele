#include "richman.h"

Richman::Richman(int id, int richmansAmount, int tunnelsAmount, size_t queueCapacity, size_t tunnelCapacity):
    info(RichmanInfo(id)),
    earthTunnel(0, queueCapacity, tunnelCapacity, Place::Earth),
    dimTunnel(1, queueCapacity, tunnelCapacity, Place::Dimension),
    walker(info, Place::Earth, tunnelsAmount),
    dispatcher(info, std::map<int, Tunnel>{
        {earthTunnel.getTunnelId(), earthTunnel},
        {dimTunnel.getTunnelId(), dimTunnel}},
               richmansAmount)
{

}

void Richman::start()
{
    this->dispatcher.start();
    this->walker.start();
}

void Richman::stop()
{
    this->dispatcher.MsgDispatcher::~MsgDispatcher();
    this->walker.TunnelWalker::~TunnelWalker();
}

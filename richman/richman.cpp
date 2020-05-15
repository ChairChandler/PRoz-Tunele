#include "richman.h"

Richman::Richman(int id, int richmansAmount, int tunnelsAmount, size_t queueCapacity, size_t tunnelCapacity):
    info(RichmanInfo(id)),
    walker(info, Place::Earth, tunnelsAmount),
    dispatcher(info, queueCapacity, tunnelCapacity, richmansAmount)
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

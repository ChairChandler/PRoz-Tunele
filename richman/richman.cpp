#include "richman.h"

Richman::Richman(int id, size_t queueMaxSize, size_t tunnelMaxSize):
    info(RichmanInfo(id)), walker(info, Place::Earth), dispatcher(info, queueMaxSize, tunnelMaxSize)
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

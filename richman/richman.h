#ifndef RICHMAN_H
#define RICHMAN_H
#include "models/richmaninfo.h"
#include "tunnelwalker.h"
#include "msgdispatcher.h"
#include <atomic>

class Richman
{
    std::atomic<RichmanInfo> info;
    TunnelWalker walker;
    MsgDispatcher dispatcher;
public:
    Richman(int id, size_t queueMaxSize, size_t tunnelMaxSize);
    void start();
    void stop();
};

#endif // RICHMAN_H

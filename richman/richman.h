#ifndef RICHMAN_H
#define RICHMAN_H
#include "thread/runnablethread.h"
#include "models/richmaninfo.h"
#include "tunnelwalker.h"
#include "msgdispatcher.h"
#include <atomic>
#include "atomicrichmaninfo.h"

class Richman
{
    AtomicRichmanInfo info;
    Tunnel earthTunnel, dimTunnel;
    TunnelWalker walker;
    MsgDispatcher dispatcher;
    RunnableThread thWalker;
    RunnableThread thDispatcher;
public:
    explicit Richman(int id, int richmansAmount,
                     size_t queueCapacity, size_t tunnelCapacity);
    void start();
    void stop();
};

#endif // RICHMAN_H

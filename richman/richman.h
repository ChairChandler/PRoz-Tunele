#ifndef RICHMAN_H
#define RICHMAN_H
#include "thread/runnablethread.h"
#include "models/richmaninfo.h"
#include "tunnelwalker.h"
#include "msgdispatcher.h"
#include <atomic>

class Richman
{
private:
    std::atomic<RichmanInfo> info;
    Tunnel earthTunnel, dimTunnel;
    TunnelWalker walker;
    MsgDispatcher dispatcher;
    RunnableThread thWalker;
    RunnableThread thDispatcher;
public:
    explicit Richman(int id, int richmansAmount, int tunnelsAmount,
                     size_t queueCapacity, size_t tunnelCapacity);
    void start();
    void stop();
};

#endif // RICHMAN_H

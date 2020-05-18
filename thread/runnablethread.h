#ifndef RUNNABLETHREAD_H
#define RUNNABLETHREAD_H
#include "runnable.h"
#include <thread>

class RunnableThread
{
    Runnable *runnable;
    std::thread *task;
public:
    RunnableThread(Runnable *runnable);
    void start();
    void stop();
};

#endif // RUNNABLETHREAD_H

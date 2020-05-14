#ifndef RUNNABLE_H
#define RUNNABLE_H
#include <thread>

class Runnable
{
    std::thread task;
protected:
    virtual void run() = 0;
public:
    Runnable();
    void start();
    void stop();
};

#endif // RUNNABLE_H

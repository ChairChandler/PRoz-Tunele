#ifndef RUNNABLE_H
#define RUNNABLE_H

class Runnable
{
protected:
    virtual void run() = 0;
    friend class RunnableThread;
};

#endif // RUNNABLE_H

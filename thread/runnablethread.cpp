#include "runnablethread.h"

RunnableThread::RunnableThread(Runnable *runnable): runnable(runnable)
{

}

void RunnableThread::start()
{
    this->task = new std::thread(&Runnable::run, this->runnable);
}

void RunnableThread::stop() {
    delete this->task;
}

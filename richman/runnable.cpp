#include "runnable.h"
#include <thread>

void Runnable::start()
{
    this->task = std::thread(&Runnable::run, this);
}

void Runnable::stop() {
    this->task.~thread();
}

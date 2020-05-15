#include "richmaninfo.h"

RichmanInfo::RichmanInfo(int id): id(id)
{

}

RichmanInfo& RichmanInfo::incrementCounter()
{
    this->counter++;
    return *this;
}

RichmanInfo& RichmanInfo::incrementCounter(int val)
{
    this->counter += val;
    return *this;
}

int RichmanInfo::getCounter() const
{
    return counter;
}

int RichmanInfo::getId() const
{
    return id;
}

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

void RichmanInfo::setCounter(int val)
{
    this->counter = val;
}

int RichmanInfo::getCounter() const
{
    return counter;
}

int RichmanInfo::getId() const
{
    return id;
}

bool RichmanInfo::operator==(const RichmanInfo &a) const
{
    return this->id == a.getId() && this->counter == a.getCounter();
}

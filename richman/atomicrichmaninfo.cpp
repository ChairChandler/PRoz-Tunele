#include "atomicrichmaninfo.h"

AtomicRichmanInfo::AtomicRichmanInfo(const RichmanInfo &info): shared(info)
{

}

RichmanInfo AtomicRichmanInfo::getInfo() const
{
    return this->shared.load();
}

void AtomicRichmanInfo::operator=(const RichmanInfo &info)
{
    this->shared.store(info);
}


AtomicRichmanInfo &AtomicRichmanInfo::incrementCounter()
{
    auto info = this->getInfo();
    info.incrementCounter();
    this->operator=(info);
    return *this;
}

AtomicRichmanInfo& AtomicRichmanInfo::incrementCounter(int val)
{
    auto info = this->getInfo();
    info.incrementCounter(val);
    this->operator=(info);
    return *this;
}

AtomicRichmanInfo &AtomicRichmanInfo::setCounter(int val)
{
    auto info = this->getInfo();
    info.setCounter(val);
    this->operator=(info);
    return *this;
}

int AtomicRichmanInfo::getCounter() const
{
    return this->shared.load().getCounter();
}

int AtomicRichmanInfo::getId() const
{
    return this->shared.load().getId();
}

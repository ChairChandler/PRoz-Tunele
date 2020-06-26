#ifndef ATOMICRICHMANINFO_H
#define ATOMICRICHMANINFO_H
#include <atomic>
#include "models/richmaninfo.h"

class AtomicRichmanInfo {
    std::atomic<RichmanInfo> shared;
public:
    explicit AtomicRichmanInfo(const RichmanInfo &info);
    RichmanInfo getInfo() const;
    AtomicRichmanInfo& incrementCounter();
    AtomicRichmanInfo& incrementCounter(int val);

    AtomicRichmanInfo& setCounter(int val);
    AtomicRichmanInfo& setIfMax(int val);
    int getCounter() const;
    int getId() const;
    void operator=(const RichmanInfo &info);
};

#endif // ATOMICRICHMANINFO_H

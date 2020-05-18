#ifndef DCOUT_H
#define DCOUT_H
#include <string>

class DistributedStream
{
public:
    DistributedStream& operator<<(std::string m);
};

inline DistributedStream dcout;

#endif // DCOUT_H

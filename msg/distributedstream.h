#ifndef DCOUT_H
#define DCOUT_H
#include <string>
#include <array>

class DistributedStream
{
public:
    static constexpr int PacketSize = 1024;
    using Packet = std::array<char, PacketSize>;

    DistributedStream& write(const std::string &m);
    DistributedStream& read(int &id, std::string &m);
private:
    Packet preparePacket(const std::string &m);
};

inline DistributedStream dstream;

#endif // DCOUT_H

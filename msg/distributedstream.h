#ifndef DCOUT_H
#define DCOUT_H
#include <string>
#include <array>

class DistributedStream
{
public:
    static constexpr int PacketSize = 1024;
    using Packet = std::array<char, PacketSize>;
    using Callback = void(*)(int id, std::string m);

    DistributedStream& write(const std::string &m);
    DistributedStream& read(Callback cb);
private:
    const int SENDING_TAG = 1000;
    const int WAITING_TAG = 1001;
    Packet preparePacket(const std::string &m);
};

inline DistributedStream dstream;

#endif // DCOUT_H

#ifndef DCOUT_H
#define DCOUT_H
#include <string>
#include <array>

class DistributedStream
{
public:
    static constexpr int PACKET_SIZE = 1024;
    using Callback = void(*)(int id, std::string m);
    using Packet = std::array<char, PACKET_SIZE>;
private:
    int receiver = 0;
    const int SENDING_TAG = 1000;
    const int WAITING_TAG = 1001;
    Packet preparePacket(const std::string &m);
public:
    void setReceiver(int recvr);
    DistributedStream& write(const std::string &m);
    DistributedStream& write(const std::string &who, const std::string &m);
    DistributedStream& read(Callback cb);
};

inline DistributedStream dstream;

#endif // DCOUT_H

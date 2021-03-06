#include "distributedstream.h"
#include <mpi.h>

void DistributedStream::setReceiver(int recvr)
{
    this->receiver = recvr;
}

DistributedStream& DistributedStream::write(const std::string &m)
{    
    static int tmp;
    MPI_Status status;
    Packet p = this->preparePacket(m);
    MPI_Send(&p, sizeof(Packet), MPI_BYTE, this->receiver, this->SENDING_TAG, MPI_COMM_WORLD);
    MPI_Recv(&tmp, 1, MPI_INT, this->receiver, this->WAITING_TAG, MPI_COMM_WORLD, &status);
    return *this;
}

DistributedStream &DistributedStream::write(const std::string &who, const std::string &m)
{
    return this->write(who + ": " + m);
}

DistributedStream::Packet DistributedStream::preparePacket(const std::string &m)
{
    Packet p;
    p.fill('\0');
    std::copy(m.begin(), m.end(), p.begin());
    return p;
}

DistributedStream& DistributedStream::read(Callback cb)
{
    static MPI_Status status;
    static int tmp;

    thread_local Packet p;
    MPI_Recv(&p, sizeof(Packet), MPI_BYTE, MPI_ANY_SOURCE, this->SENDING_TAG, MPI_COMM_WORLD, &status);
    cb(status.MPI_SOURCE, std::string(p.data()));
    MPI_Send(&tmp, 1, MPI_INT, status.MPI_SOURCE, this->WAITING_TAG, MPI_COMM_WORLD);
    return *this;
}

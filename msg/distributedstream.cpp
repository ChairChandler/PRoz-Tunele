#include "distributedstream.h"
#include <mpi.h>

DistributedStream& DistributedStream::write(const std::string &m)
{    
    Packet p = this->preparePacket(m);
    MPI_Send(&p, sizeof(Packet), MPI_BYTE, 0, 1000, MPI_COMM_WORLD);
    return *this;
}

DistributedStream::Packet DistributedStream::preparePacket(const std::string &m)
{
    static Packet p;
    p.fill('\0');
    std::copy(m.begin(), m.end(), p.begin());
    return p;
}

DistributedStream& DistributedStream::read(int &id, std::string &m)
{
    static MPI_Status status;

    static Packet p;
    MPI_Recv(&p, sizeof(Packet), MPI_BYTE, MPI_ANY_SOURCE, 1000, MPI_COMM_WORLD, &status);
    id = status.MPI_SOURCE;
    m = std::string(p.data());
    return *this;
}

#include "distributedstream.h"
#include <mpi.h>

DistributedStream &DistributedStream::operator<<(std::string m)
{
    int size = m.size();
    MPI_Send(&size, 1, MPI_INT, 0, 3, MPI_COMM_WORLD);
    MPI_Send(m.data(), size, MPI_CHAR, 0, 3, MPI_COMM_WORLD);
    return *this;
}

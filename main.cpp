#include <mpi.h>
#include "richman/richman.h"

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int id, richmansAmount;
    //const int richmanGroupSize = 1;
    const int tunnelsAmount = 2, tunnelCapacity = 2, queueCapacity = 5;

    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &richmansAmount);

    Richman richman(id, richmansAmount, tunnelsAmount, queueCapacity, tunnelCapacity);
    richman.start();

    MPI_Finalize();
    return 0;
}

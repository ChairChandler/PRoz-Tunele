//#include <mpi/mpi.h>
#include "richman/richman.h"
#include <vector>

int main(int argc, char *argv[])
{
    //MPI_Init(&argc, &argv);

    const int id = 0;
    const int richmansAmount = 10, richmanGroupSize = 1;
    const int tunnelAmount = 2, tunnelCapacity = 2, queueCapacity = 5;

    Richman richman(id, queueCapacity, tunnelCapacity);
    richman.start();

    //MPI_Finalize();
    return 0;
}

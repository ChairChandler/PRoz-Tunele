#include <mpi.h>
#include "richman/richman.h"
#include "msg/distributedstream.h"

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    int id, richmansAmount;
    //const int richmanGroupSize = 1;
    const int tunnelsAmount = 2, tunnelCapacity = 2, queueCapacity = 5;

    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &richmansAmount);

    if(id) {
        Richman richman(id - 1, richmansAmount - 1, tunnelsAmount, queueCapacity, tunnelCapacity);
        richman.start();
        while(true);
    } else {
        while(true) {
            int src;
            std::string log;
            dstream.read(src, log);
            std::cout << "[" << src << "] " << log << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}

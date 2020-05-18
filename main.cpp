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

    if(id) {
        Richman richman(id - 1, richmansAmount - 1, tunnelsAmount, queueCapacity, tunnelCapacity);
        richman.start();
        while(true);
    } else {
        std::vector<char> buff;
        int size;
        MPI_Status status;

        while(true) {
            MPI_Recv(&size, 1, MPI_INT, MPI_ANY_SOURCE, 3, MPI_COMM_WORLD, &status);
            buff.resize(size);
            MPI_Recv(buff.data(), size, MPI_CHAR, status.MPI_SOURCE, 3, MPI_COMM_WORLD, &status);
            buff.push_back('\0');
            std::cout << "[" << status.MPI_SOURCE << "] " << buff.data() << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}

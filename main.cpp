#include <mpi.h>
#include "richman/richman.h"
#include "utils/distributedstream.h"
#include <chrono>

int main(int argc, char *argv[])
{
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

    int id, richmansAmount;
    //const int richmanGroupSize = 1;
    const int tunnelCapacity = 2, queueCapacity = 5;

    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &richmansAmount);

    dstream.setReceiver(richmansAmount - 1);

    if(id < (richmansAmount - 1)) {
        Richman richman(id, richmansAmount - 1, queueCapacity, tunnelCapacity);
        richman.start();
        while(true);
    } else {
        while(true) {
            dstream.read([](int src, std::string log) {
                std::cout << "[" << src << "] " << log << std::endl;
            });
        }
    }

    MPI_Finalize();
    return 0;
}

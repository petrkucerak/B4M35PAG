#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    int data[4]; // Data to be scattered
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size( MPI_COMM_WORLD, &size);
    if (rank == 0) {
        data[0] = 11; // Data to be scattered for process 1
        data[1] = 22; // Data to be scattered for process 2
        data[2] = 33; // Data to be scattered for process 3
        data[3] = 44; // Data to be scattered for process 4
    }

    MPI_Bcast( data , 4, MPI_INT , 0, MPI_COMM_WORLD);

    printf("Process %d received data: {%d, %d, %d, %d}\n", rank, data[0], data[1], data[2], data[3]);

    MPI_Finalize();
    return 0;
}
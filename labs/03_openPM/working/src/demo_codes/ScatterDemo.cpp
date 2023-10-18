#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, data, size;
    int scatter_data[4]; // Data to be scattered
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size( MPI_COMM_WORLD, &size);
    if (rank == 0) {
        scatter_data[0] = 11; // Data to be scattered for process 1
        scatter_data[1] = 22; // Data to be scattered for process 2
        scatter_data[2] = 33; // Data to be scattered for process 3
        scatter_data[3] = 44; // Data to be scattered for process 4
    }

    MPI_Scatter(scatter_data, 1, MPI_INT, &data, 4, MPI_INT, 0, MPI_COMM_WORLD);

    printf("Process %d received data: %d\n", rank, data);

    MPI_Finalize();
    return 0;
}
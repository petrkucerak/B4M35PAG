#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, data, size;
    int gather_data[4]; // Data to be scattered
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size( MPI_COMM_WORLD, &size);

    data = 11*(rank+1);

    MPI_Gather(&data , 1 , MPI_INT, gather_data , 1, MPI_INT , 0 , MPI_COMM_WORLD);
    if (rank == 0) {
        printf("\nProcess %d recieved data {%d, %d, %d, %d}\n", rank, gather_data[0], gather_data[1], gather_data[2], gather_data[3]);
    }

    MPI_Finalize();
    return 0;
}
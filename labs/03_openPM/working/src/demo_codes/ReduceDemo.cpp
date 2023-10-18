#include <stdio.h>
#include "mpi.h"

int main(int argc, char **argv) {
    int size, rank;
    int data;
    int result;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //Prepare local data
    data = rank*2+1;
    printf("Process %d: has data %d\n", rank, data);
    
    MPI_Reduce(&data, &result, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Process %d has the result: %d\n", rank, result);
    }

    MPI_Finalize();
    return 0;
}
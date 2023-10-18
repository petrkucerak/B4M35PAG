#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
   MPI_Init(&argc, &argv);

   int rank, size;
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &size);

   int vectorSize = 16;               // Vector size
   int chunkSize = vectorSize / size; // Size of vector chunk for each process

   double *localVector = (double *)malloc(chunkSize * sizeof(double));
   double *globalVector = (double *)malloc(vectorSize * sizeof(double));

   if (rank == 0) {
      // Process with rank 0 generates vector
      for (int i = 0; i < vectorSize; i++) {
         globalVector[i] = 2;
      }
   }

   // TODO

   // Printing vector on process with rank 0
   if (rank == 0) {
      printf("Normalized Vector: {");
      for (int i = 0; i < vectorSize; i++) {
         printf("%.2f ", globalVector[i]);
      }
      printf("}\n");
   }

   MPI_Finalize();

   return 0;
}
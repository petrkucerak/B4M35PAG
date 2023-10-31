#include "mpi.h"
#include "stdio.h"
#include "stdlib.h"
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>

using namespace std;
using namespace std::chrono;

int main(int argc, char **argv)
{
   int rank, size, N;
   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   float *matrix;

   // Generate matrix
   if (rank == 0) {
      cout << "\nEnter matrix size: ";
      cin >> N;
      cout << "\n";

      // Broadcast of the matrix size
      MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

      matrix = (float *)malloc(N * N * sizeof(float));
      if (N <= 8) {
         cout << "Original matrix: " << endl;
      }
      for (int i = 0; i < N; i++) {
         for (int j = 0; j < N; j++) {
            matrix[i * N + j] =
                static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
            if (N <= 8) {
               cout << std::fixed << std::setprecision(2) << matrix[i * N + j]
                    << " ";
            }
         }
         if (N <= 8) {
            cout << endl;
         }
      }
      if (N <= 8) {
         cout << endl;
      }
   } else {
      // Use broadcast to receive the size of a matrix
      MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
   }

   // Create buffer subMatrix for matrix chunk at each process
   int num_rows = N / size;
   float *sub_matrix = new float[N * num_rows];
   float *row = new float[N];

   std::chrono::_V2::system_clock::time_point t_start;
   if (rank == 0) {
      t_start = (high_resolution_clock::now());
   }

   /*
   - Scatter the matrix between all processes use the buffer
       - Iterate through rows and scatter matrix chunks
       - One chunk equals to number of rows of the communicator size
   - Perform row update for each process. There are 2 possible scenarios:
       - Get the pivot row index using modulo operation
           * If result equals to process rank, then process has the pivot
       - Pivot row is one of the rows that is operating with
           * Eliminate pivot row and do elimination algorithm for rest of the
   rows
       - Pivot row is not one of the rows that is operating with
           * Receive the Pivot row and do elimination algorithm
   - Gather the eliminated chunks at process 0
       - Use the same system as for the row scatter
   */

   if (size == 1) {
      memcpy(sub_matrix, matrix, N * N * sizeof(float));
   } else {
      for (int k = 0; k < num_rows; k++) {
         MPI_Scatter(&matrix[k * N * size], N, MPI_FLOAT, &sub_matrix[k * N], N,
                     MPI_FLOAT, 0, MPI_COMM_WORLD);
      }
   }

   int local_row, pivot_rank;
   float pivot, scale;
   // Iteration is through all rows
   for (int k = 0; k < N; k++) {
      local_row = k / size;
      pivot_rank = (k % size);

      if (rank == pivot_rank) {
         pivot = sub_matrix[local_row * N + k];
         for (int i = k + 1; i < N; i++) {
            sub_matrix[local_row * N + i] /= pivot;
         }

         sub_matrix[local_row * N + k] = 1;

         memcpy(row, &sub_matrix[local_row * N], N * sizeof(float));

         MPI_Bcast(row, N, MPI_FLOAT, pivot_rank, MPI_COMM_WORLD);

         for (int i = local_row + 1; i < num_rows; i++) {
            scale = sub_matrix[i * N + k];
            for (int j = k + 1; j < N; j++) {
               sub_matrix[i * N + j] -= scale * row[j];
            }
            sub_matrix[i * N + k] = 0;
         }
      } else {
         MPI_Bcast(row, N, MPI_FLOAT, pivot_rank, MPI_COMM_WORLD);
         for (int i = local_row; i < num_rows; i++) {
            if ((pivot_rank < rank) || (i > local_row)) {
               scale = sub_matrix[i * N + k];
               for (int j = k + 1; j < N; j++) {
                  sub_matrix[i * N + j] -= scale * row[j];
               }
               sub_matrix[i * N + k] = 0;
            }
         }
      }
   }
   MPI_Barrier(MPI_COMM_WORLD);

   if (size == 0) {
      memcpy(matrix, sub_matrix, N * N * sizeof(float));
   } else {
      for (int k = 0; k < num_rows; k++) {
         MPI_Gather(&sub_matrix[k * N], N, MPI_FLOAT, &matrix[k * size * N], N,
                    MPI_FLOAT, 0, MPI_COMM_WORLD);
      }
   }

   if (rank == 0) {
      auto t_end = (high_resolution_clock::now());
      double time = duration_cast<duration<double>>(t_end - t_start).count();
      printf("Total time: %f\n\n", (time));
      if (N <= 8) {
         cout << "Matrix after Gauss Elimination : " << endl;
         for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
               cout << std::fixed << std::setprecision(2) << matrix[i * N + j]
                    << " ";
            }
            cout << endl;
         }
      }
   }
   MPI_Finalize();
   return 0;
}

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

   /*
   - Create buffer subMatrix for matrix chunk at each process
   */
   int num_rows = N / size;
   float *sub_matrix = new float[N * num_rows];
   float *row = new float[N];

   std::chrono::_V2::system_clock::time_point t_start;
   if (rank == 0) {
      t_start = (high_resolution_clock::now());
   }

   // Scatter the matrix between all processes use the buffer
   MPI_Scatter(matrix, N * num_rows, MPI_FLOAT, sub_matrix, N * num_rows,
               MPI_FLOAT, 0, MPI_COMM_WORLD);

   // Perform row update for each process. There are 3 possible scenarios:
   // 1. Pivot row is above all rows that process is operating with
   //  Receive the Pivot row and do elimination algorithm

   for (int k = 0; k < (rank * num_rows); k++) {
      // Receive row
      MPI_Bcast(row, N, MPI_FLOAT, k / num_rows, MPI_COMM_WORLD);
      for (int i = 0; i < num_rows; i++) {
         float scale = sub_matrix[i * N + k];
         for (int j = k + 1; j < N; j++) {
            sub_matrix[i * N + j] -= (scale * row[j]);
         }
         sub_matrix[i * N + k] = 0;
      }
   }
   // 2. Pivot row is one of the rows that is operating with
   //  Eliminate pivot row and do elimination algorithm for rest of the rows
   int pivot_colum;
   for (int k = 0; k < num_rows; k++) {
      pivot_colum = (rank * num_rows) + k;
      float pivot = sub_matrix[k * N + pivot_colum];
      for (int i = pivot_colum + 1; i < N; i++) {
         sub_matrix[k * N + i] /= pivot;
      }
      sub_matrix[k * N + pivot_colum] = 1;
      memcpy(row, &sub_matrix[k * N], N * sizeof(float));
      MPI_Bcast(row, N, MPI_FLOAT, rank, MPI_COMM_WORLD);
      for (int i = k + 1; i < num_rows; i++) {
         float scale = sub_matrix[i * N + pivot_colum];
         for (int j = pivot_colum + 1; j < N; j++) {
            sub_matrix[i * N + j] -= scale * row[j];
         }
         sub_matrix[i * N + pivot_colum] = 0;
      }
   }
   // 3. Pivot row is bellow the rows that process is operating with
   //  Wait -> Broadcast is synchronous so receive the row to prevent deadlocks
   for (int k = rank * num_rows + 1; k < N; k++) {
      MPI_Bcast(row, N, MPI_FLOAT, k / num_rows, MPI_COMM_WORLD);
   }

   // Gather the eliminated chunks at process 0
   MPI_Gather(sub_matrix, N * num_rows, MPI_FLOAT, matrix, N * num_rows,
              MPI_FLOAT, 0, MPI_COMM_WORLD);

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

#include "stdio.h"
#include "stdlib.h"
#include "mpi.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;

int main(int argc, char **argv) {
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

        // TODO implement: Broadcast of the matrix size

        matrix = (float *) malloc(N * N * sizeof(float));
        if (N <= 8) {
            cout << "Original matrix: " << endl;
        }
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                matrix[i * N + j] = static_cast<float> (rand()) / static_cast<float> (RAND_MAX);
                if (N <= 8) {
                    cout << std::fixed << std::setprecision(2) << matrix[i * N + j] << " ";
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
        // TODO implement: Use broadcast to receive the size of a matrix
    }

    // TODO implement:
    /*
    - Create buffer subMatrix for matrix chunk at each process
    */

    std::chrono::_V2::system_clock::time_point t_start;
    if (rank == 0) {
        t_start = (high_resolution_clock::now());
    }

    // TODO implement:
    /*
    - Scatter the matrix between all processes use the buffer
        - Iterate through rows and scatter matrix chunks
        - One chunk equals to number of rows of the communicator size
    - Perform row update for each process. There are 2 possible scenarios:
        - Get the pivot row index using modulo operation
            * If result equals to process rank, then process has the pivot
        - Pivot row is one of the rows that is operating with
            * Eliminate pivot row and do elimination algorithm for rest of the rows
        - Pivot row is not one of the rows that is operating with
            * Receive the Pivot row and do elimination algorithm
    - Gather the eliminated chunks at process 0
        - Use the same system as for the row scatter
    */

    if (rank == 0) {
        auto t_end = (high_resolution_clock::now());
        double time = duration_cast<duration<double>>(t_end - t_start).count();
        printf("Total time: %f\n\n", (time));
        if (N <= 8) {
            cout << "Matrix after Gauss Elimination : " << endl;
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    cout << std::fixed << std::setprecision(2) << matrix[i * N + j] << " ";
                }
                cout << endl;
            }
        }
    }
    MPI_Finalize();
    return 0;
}

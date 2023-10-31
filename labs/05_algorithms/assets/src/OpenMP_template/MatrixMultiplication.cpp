#include <iostream>
#include <cstdlib>
#include <chrono>
#include <omp.h>

#pragma comment (lib, "msmpi.lib")

using namespace std;
using namespace std::chrono;

void matrix_mul(float **A, float **B, float **C, int size) {
    int i, j, k;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            for (k = 0; k < size; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void matrix_mul_transposed(float **A, float **BT, float **C, int size) {
    int i, j, k;
    for (i = 0; i < size; i++) {
        for (j = 0; j < size; j++) {
            for (k = 0; k < size; k++) {
                C[i][j] += A[i][k] * BT[j][k];
            }
        }
    }
}

void matrix_mul_parallel(float **A, float **B, float **C, int size)
{
    // TODO implement
    /*
    - Implement parallel matrix multiplication.
    - Use provide code from serial version as a template
    - Beware race conditions
        - Use shared variables for matrices
        - Use private variable for iterable variables
    */
}

void matrix_mul_parallel_transposed(float **A, float **BT, float **C, int size)
{
    // TODO implement
    /*
    - Implement parallel matrix multiplication using transposition of matrix on the right
    - Use provide code from serial version as a template
    - Beware race conditions
        - Use shared variables for matrices
        - Use private variable for iterable variables
    */
}

void block_matrix_mul(float **A, float **B, float **C, int size, int block_size)
{
    // TODO implement
    /*
    - Implement block matrix multiplication
    - You are going to need 4 inner loops
        - Outer 2 for the block iteration
        - Inner 2 for matrix multiplication within the block
    */
}

void block_matrix_mul_transposed(float **A, float **BT, float **C, int size, int block_size)
{
    // TODO implement
    /*
    - Implement block matrix multiplication using transposition of matrix on the right
    - You are going to need 4 inner loops
        - Outer 2 for the block iteration
        - Inner 2 for matrix multiplication within the block
    */
}

void block_matrix_mul_parallel(float **A, float **B, float **C, int size, int block_size)
{
    // TODO implement
    /*
    - Implement parallel block matrix multiplication
    - Use your code of the serial version
    - Beware of race conditions same as above
    */
}

void make_transpose_matrix(float **B, float **BT, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            BT[i][j] = B[j][i];
        }
    }
}

void output_matrix(float **matrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            cout << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

int main(int argc, char **argv) {
    int size, block_size;
    cout << "Enter size of Matrix: ";
    cin >> size;
    cout << "Enter size of block: ";
    cin >> block_size;
    float **A = new float *[size];
    float **B = new float *[size];
    float **BT = new float *[size];
    float **C = new float *[size];
    float **CC = new float *[size];
    cout << "Init has begun" << endl;
    for (int i = 0; i < size; i++) {
        A[i] = new float[size];
        B[i] = new float[size];
        BT[i] = new float[size];
        C[i] = new float[size];
        CC[i] = new float[size];
        for (int j = 0; j < size; j++) {
            A[i][j] = static_cast<float> (rand()) / static_cast<float> (RAND_MAX);
            B[i][j] = static_cast<float> (rand()) / static_cast<float> (RAND_MAX);
            C[i][j] = 0.0f;
            CC[i][j] = 0.0f;
        }
    }

    make_transpose_matrix(B, BT, size);

    for (int it = 0; it < 1; it++) {
        cout << "Computation has begun(serial)" << endl;
        high_resolution_clock::time_point t1 = high_resolution_clock::now();

        matrix_mul(A, B, C, size);

        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        cout << "Time elapsed: " << static_cast <float> (duration) / 1000000.0f << endl;

        cout << "Computation has begun(serial transposed)" << endl;
        t1 = high_resolution_clock::now();

        matrix_mul_transposed(A, BT, C, size);

        t2 = high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        cout << "Time elapsed: " << static_cast <float> (duration) / 1000000.0f << endl;

        cout << "Computation has begun(parallel)" << endl;
        t1 = high_resolution_clock::now();

        matrix_mul_parallel(A, B, C, size);

        t2 = high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        cout << "Time elapsed: " << static_cast <float> (duration) / 1000000.0f << endl;

        cout << "Computation has begun(parallel transposed)" << endl;
        t1 = high_resolution_clock::now();

        matrix_mul_parallel_transposed(A, BT, C, size);

        t2 = high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        cout << "Time elapsed: " << static_cast <float> (duration) / 1000000.0f << endl;

        cout << "Computation has begun(serial block)" << endl;
        t1 = high_resolution_clock::now();

        block_matrix_mul(A, B, C, size, block_size);

        t2 = high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        cout << "Time elapsed: " << static_cast <float> (duration) / 1000000.0f << endl;

        cout << "Computation has begun(serial block, transposed)" << endl;
        t1 = high_resolution_clock::now();

        block_matrix_mul_transposed(A, BT, C, size, block_size);

        t2 = high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        cout << "Time elapsed: " << static_cast <float> (duration) / 1000000.0f << endl;

        cout << "Computation has begun(parallel block)" << endl;
        t1 = high_resolution_clock::now();

        block_matrix_mul_parallel(A, B, CC, size, block_size);

        t2 = high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        cout << "Time elapsed: " << static_cast <float> (duration) / 1000000.0f << endl;
    }
    return 0;
}

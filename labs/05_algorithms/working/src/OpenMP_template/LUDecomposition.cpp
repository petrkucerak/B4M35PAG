#include <iostream>
#include <cmath>
#include <omp.h>
#include <chrono>
#include <iomanip>

#define VERBOSE true
#define VALIDATION true
#define DEBUG true

using namespace std;
using namespace std::chrono;

// initialize the matrices
inline void initialize_matrices(float **a, float **l, float **u, int size) {
#pragma omp for schedule(static)
    for (int i = 0; i < size; ++i) {
        a[i] = new float[size];
        l[i] = new float[size];
        u[i] = new float[size];
    }
}

bool matrix_validation(float **a, float **l, float **u, int size) {
    float **check = new float *[size];
    float **a2 = new float *[size];
    float **l2 = new float *[size];
    float **u2 = new float *[size];
    initialize_matrices(a2, l2, u2, size);
    for (int i = 0; i < size; ++i) {
        check[i] = new float[size];
    }

    l2 = l;
    u2 = u;

    // matrix multiplication
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            check[i][j] = 0;
            for (int k = 0; k < size; k++) {
                check[i][j] += u2[k][j] * l2[i][k];
            }
        }
    }
    int error = 0;
    cout << "Validation results: " << endl;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (abs(check[i][j] - a[i][j]) > 1e8) {
                error = 1;
                cout << "Error index is: " << (check[i][j] - a[i][j]) << endl;
            }

        }
    }

    if (error == 1) {
        cout << "Failed" << endl;
        return false;
    } else {
        cout << "Success" << endl;
        return true;
    }
}

int main(int argc, char **argv) {
    int size, chunk_size;
    // user input section end
    cout << "Enter matrix size: ";
    cin >> size;
    float **a = new float *[size];
    float **l = new float *[size];
    float **u = new float *[size];

    initialize_matrices(a, l, u, size);

    if (size <= 8) {
        cout << "Matrix A:" << endl;
    }
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            a[i][j] = static_cast<float> (rand()) / static_cast<float> (RAND_MAX);
            if (size <= 8) {
                cout << std::fixed << std::setprecision(2) << a[i][j] << " ";
            }
        }
        if (size <= 8) {
            cout << endl;
        }
    }
    chunk_size = 1;
#if defined(_OPENMP)
    omp_set_num_threads(16);
#endif
    auto t_start = (high_resolution_clock::now());

    // TODO implement:
    /*
	- Implement parallel LU decomposition of matrix A
	- You will have to iterate throw each row
		- For L matrix set for given row each element above the diagonal to 0
		- For U matrix set for given row each element above the diagonal to 0
		- For L matrix set diagonal element to 1
		- Use equations from the slides to update rest of the row elements of matrix L
		- Use equations from the slides to update rest of the row elements of matrix U
	- Use simd operations and dynamic schedule for the best performance
	*/

    auto t_end = (high_resolution_clock::now());
    double time = duration_cast<duration<double>>(t_end - t_start).count();
    cout << "LU Decomposition time: " << time << " seconds" << endl;
    if (matrix_validation(a, l, u, size)) {
        if (size <= 8) {
            cout << "Matrix L: " << endl;
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    cout << std::fixed << std::setprecision(2) << l[i][j] << " ";
                }
                cout << endl;
            }
            cout << "Matrix U: " << endl;
            for (int i = 0; i < size; i++) {
                for (int j = 0; j < size; j++) {
                    cout << std::fixed << std::setprecision(2) << u[i][j] << " ";
                }
                cout << endl;
            }
        }
    }
    return 0;
}

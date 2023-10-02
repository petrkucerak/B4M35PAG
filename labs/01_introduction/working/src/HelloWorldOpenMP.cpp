#include <iostream>
#include <vector>
#include <omp.h>

using namespace std;

int main() {
    #pragma omp parallel
    {
        #pragma omp single
        {
            cout << "Number of available threads "<< omp_get_num_threads() << endl;
        }

        #pragma omp critical
        {
            cout << "This is thread " << omp_get_thread_num() << " speaking" << endl;
        }
    }
    cout << "Parallel block finished" << endl;
    
    return 0;
}

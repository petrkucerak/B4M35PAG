#include <iostream>
#include <vector>
#include <omp.h>
#include <cmath>

#include "Utils.hpp"

using namespace std;

void mergeSortSequential(vector<double> &u) {
    // TODO: you can use inplace_merge() from <algorithm> for merging arrays.
}

void mergeSortParallel(vector<double> &u) {
    // TODO: you can use inplace_merge() from <algorithm> for merging arrays.
}

int main() {
    vector<double> u = generateRandomVector(5000000);

    {
        auto uCopy = u;
        Stopwatch sw;
        sw.start();
        mergeSortSequential(uCopy);
        sw.stop();
        cout << "Sequential merge sort: " << sw.duration().count()
             << " ms, sorted correctly " << is_sorted(uCopy.begin(), uCopy.end()) << endl;
    }

    {
        auto uCopy = u;
        Stopwatch sw;
        sw.start();
        mergeSortParallel(uCopy);
        sw.stop();
        cout << "Parallel merge sort: " << sw.duration().count()
             << " ms, sorted correctly " << is_sorted(uCopy.begin(), uCopy.end()) << endl;
    }

    return 0;
}

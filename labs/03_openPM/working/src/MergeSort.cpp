#include <algorithm>
#include <cmath>
#include <iostream>
#include <omp.h>
#include <vector>

#include "Utils.hpp"

using namespace std;

void mergeSortSequentialRecursion(vector<double> &u, uint64_t left,
                                  uint64_t right)
{
   if (left < right) {
      if (right - left > 32) {
         uint64_t mid = (left + right) / 2;
         mergeSortSequentialRecursion(u, left, mid);
         mergeSortSequentialRecursion(u, mid + 1, right);
         inplace_merge(u.begin() + left, u.begin() + mid + 1,
                       u.begin() + right + 1);
      } else {
         sort(u.begin() + left, u.begin() + right + 1);
      }
   }
}

void mergeSortSequential(vector<double> &u)
{
   mergeSortSequentialRecursion(u, 0, u.size() - 1);
}

void mergeSortParallelRecursion(vector<double> &u, uint64_t left,
                                uint64_t right)
{
   if (left < right) {
      if (right - left > 32) {
         uint64_t mid = (left + right) / 2;
         #pragma omp taskgroup
         {
            #pragma omp task shared(u)
            mergeSortParallelRecursion(u, left, mid);
            #pragma omp task shared(u)
            mergeSortParallelRecursion(u, mid + 1, right);
         }

         inplace_merge(u.begin() + left, u.begin() + mid + 1,
                       u.begin() + right + 1);
      } else {
         sort(u.begin() + left, u.begin() + right + 1);
      }
   }
}

void mergeSortParallel(vector<double> &u)
{
   #pragma omp parallel
   {
      #pragma omp single
      {
         mergeSortParallelRecursion(u, 0, u.size() - 1);
      }
   }
}

int main()
{
   vector<double> u = generateRandomVector(5000000);

   {
      auto uCopy = u;
      Stopwatch sw;
      sw.start();
      mergeSortSequential(uCopy);
      sw.stop();
      cout << "Sequential merge sort: " << sw.duration().count()
           << " ms, sorted correctly " << is_sorted(uCopy.begin(), uCopy.end())
           << endl;
   }

   {
      auto uCopy = u;
      Stopwatch sw;
      sw.start();
      mergeSortParallel(uCopy);
      sw.stop();
      cout << "Parallel merge sort: " << sw.duration().count()
           << " ms, sorted correctly " << is_sorted(uCopy.begin(), uCopy.end())
           << endl;
   }

   return 0;
}

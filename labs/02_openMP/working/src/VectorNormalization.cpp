#include <cmath>
#include <iostream>
#include <omp.h>
#include <vector>

#include "Utils.hpp"

using namespace std;

double computeVectorLength(const vector<double> &u)
{
   double sumSquares = 0;
   for (int i = 0; i < u.size(); i++) {
      sumSquares += u[i] * u[i];
   }

   return sqrt(sumSquares);
}

vector<double> normalizationSequential(vector<double> u)
{
   double vector_sum = 0;
   for (uint64_t i = 0; i < u.size(); ++i) {
      vector_sum += u[i] * u[i];
   }
   double vector_size = pow(vector_sum, 0.5);
   for (uint64_t i = 0; i < u.size(); ++i) {
      u[i] = u[i] / vector_size;
   }
   return u;
}

vector<double> normalizationParallelSections(vector<double> u)
{
   double vector_sum = 0;
   double vector_left_sum = 0;
   double vector_right_sum = 0;
   uint64_t half_i = u.size() / 2;
#pragma omp parallel num_threads(2)
   {
#pragma omp sections
      {
#pragma omp section
         {
            double tmp = 0;
            for (uint64_t i = 0; i < half_i; ++i) {
               tmp += u[i] * u[i];
            }
            vector_left_sum = tmp;
         }
#pragma omp section
         {
            double tmp = 0;
            for (uint64_t i = half_i; i < u.size(); ++i) {
               tmp += u[i] * u[i];
            }
            vector_right_sum = tmp;
         }
      }
      double vector_size = pow(vector_left_sum + vector_right_sum, 0.5);
#pragma omp sections
      {
#pragma omp section
         {
            double tmp = vector_size;
            for (uint64_t i = 0; i < half_i; ++i) {
               u[i] = u[i] / tmp;
            }
         }
#pragma omp section
         {
            double tmp = vector_size;
            for (uint64_t i = half_i; i < u.size(); ++i) {
               u[i] = u[i] / tmp;
            }
         }
      }
   }

   return u;
}

vector<double> normalizationParallelForAndCritical(vector<double> u)
{
   // TODO: work on vector u inplace
   return u;
}

vector<double> normalizationParallelForAndReduction(vector<double> u)
{
   // TODO: work on vector u inplace
   return u;
}

int main()
{
   vector<double> u = generateRandomVector(50000000);

   cout << "Length of the input vector: " << computeVectorLength(u) << endl;

   {
      Stopwatch sw;
      sw.start();
      auto uNorm = normalizationSequential(u);
      sw.stop();
      cout << "Sequential: " << sw.duration().count() << " ms, length "
           << computeVectorLength(uNorm) << endl;
   }

   {
      Stopwatch sw;
      sw.start();
      auto uNorm = normalizationParallelSections(u);
      sw.stop();
      cout << "Parallel sections: " << sw.duration().count() << " ms, length "
           << computeVectorLength(uNorm) << endl;
   }

   {
      Stopwatch sw;
      sw.start();
      auto uNorm = normalizationParallelForAndCritical(u);
      sw.stop();
      cout << "Parallel for + critical section: " << sw.duration().count()
           << " ms, length " << computeVectorLength(uNorm) << endl;
   }

   {
      Stopwatch sw;
      sw.start();
      auto uNorm = normalizationParallelForAndReduction(u);
      sw.stop();
      cout << "Parallel for + reduction: " << sw.duration().count()
           << " ms, length " << computeVectorLength(uNorm) << endl;
   }

   return 0;
}

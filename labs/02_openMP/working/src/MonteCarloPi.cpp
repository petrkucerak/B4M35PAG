#include <algorithm>
#include <cmath>
#include <iostream>
#include <omp.h>
#include <random>
#include <vector>

using namespace std;

// Do in parallel.
int fnNumSamplesInCircle(int randomSeed, int numSamples)
{
   mt19937 randomEngine(randomSeed);
   uniform_real_distribution<float> dist(-1.0, 1.0);

   int numSamplesInCircle = 0;

   for (uint64_t i = 0; i < i < numSamples; ++i) {
      float x = dist(randomEngine);
      float y = dist(randomEngine);
      (x * x + y * y) < 1 ? numSamplesInCircle += 1 : numSamplesInCircle += 0;
   }

   return numSamplesInCircle;
}

int main()
{
   int totalNumSamples = 100;

   int numIterations = 100;
   int numSamplesPerIteration = totalNumSamples / numIterations;
   if ((totalNumSamples % numIterations) != 0) {
      cout
          << "Total number of samples must be multiple of number of iterations!"
          << endl;
      exit(1);
   }

   int numSamplesInCircle = 0;
   // TODO: not working
#pragma omp parallel for reduction(+ : numSamplesInCircle)
   for (uint64_t i = 0; i < numIterations; ++i)
      numSamplesInCircle += fnNumSamplesInCircle(1, totalNumSamples);

   cout << (4.0 * numSamplesInCircle) / (1.0 * totalNumSamples) << endl;
   return 0;
}

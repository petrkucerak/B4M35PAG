#include <cstring>
#include <iostream>
#include <limits.h>
#include <omp.h>
#include <queue>
#include <vector>

#include "Utils.h"

// #define SEQUENTIA

#define GRAPH(x, y) *(graph + (x) + ((y) * number_vertices))
#define MIN(X, Y, Z)                                                           \
   (((X) < (Y)) ? ((X) < (Z) ? (X) : (Z)) : ((Y) < (Z) ? (Y) : (Z)))

using namespace std;

void printGraph(int *graph, int number_vertices)
{
   for (auto i = 0; i < number_vertices; ++i) {
      for (auto j = 0; j < number_vertices; ++j)
         printf("%2d ", GRAPH(i, j));
      cout << endl;
   }
}

int main(int argc, char *argv[])
{
   auto programArguments = ProgramArguments::Parse(argc, argv);

   auto clk_start = std::chrono::high_resolution_clock::now();

   // The input records, e.g., records[0] is the first record in the input file.
   vector<vector<int>> records = readRecords(programArguments.mInputFilePath);
   // vector<vector<int>> records = {{1, 2, 5, 5, 5, 5, 3, 2}, {1, 4, 3, 2}};

   const int number_vertices = records.size();
   int *graph = (int *)malloc(sizeof(int) * number_vertices * number_vertices);
   // if (graph == NULL) {
   //    fprintf(stderr, "ERROR: Cannot allocate memory for graph memory
   //    block\n"); return 1;
   // }

   // good only for prints
   // for (auto i = 0; i < number_vertices; ++i) {
   //    for (auto j = 0; j < number_vertices; ++j)
   //       GRAPH(i, j) = 0;
   // }

#ifndef SEQUENTIA
#pragma omp parallel for /*collapse(2)*/ shared(graph, records)
#endif
   for (int i = 0; i < number_vertices; ++i) {
      for (int j = i + 1; j < number_vertices; ++j) {

         // TODO: implement L here to minimalize cost by calling a function
         const int x_size = records[i].size();
         const int y_size = records[j].size();

         vector<int> top_array(y_size + 1, 0);
         vector<int> bottom_array(y_size + 1, 0);
         for (int x = 0; x <= y_size; ++x) {
            top_array[x] = x;
         }

         for (int y = 1; y <= x_size; ++y) {
            bottom_array[0] = y;

            for (int x = 1; x <= y_size; ++x) {
               if (records[i][y - 1] == records[j][x - 1])
                  bottom_array[x] = top_array[x - 1];
               else
                  bottom_array[x] =
                      //   MIN(bottom_array[x - 1], top_array[x], top_array[x -
                      //   1]) + 1;
                      min(bottom_array[x - 1],
                          min(top_array[x], top_array[x - 1])) +
                      1;
            }
            top_array = bottom_array;
         }
         GRAPH(j, i) = bottom_array[y_size];
         // GRAPH(i, j) = bottom_array[y_size]; // SYMETRIC DUPLICITY
      }
   }

   // printGraph(graph, number_vertices);
   auto clk_stop = std::chrono::high_resolution_clock::now();

   // distances graph
   int treeCost = 0;

   vector<bool> is_edge_used(number_vertices);
   vector<int> edge_costs(number_vertices);

   is_edge_used[0] = true;
   edge_costs[0] = 0;

#ifndef SEQUENTIA
#pragma omp parallel for simd
#endif
   for (int i = 1; i < number_vertices; ++i) {
      edge_costs[i] = graph[i];
   }
   for (int j = 0; j < number_vertices - 1; ++j) {
      int min_i = -1;
      int min_val = INT_MAX;
      for (int i = 1; i < number_vertices; ++i) {
         if (!is_edge_used[i] && edge_costs[i] < min_val) {
            min_val = edge_costs[i];
            min_i = i;
         }
      }
      is_edge_used[min_i] = true;
      for (int i = 1; i < number_vertices; ++i) {
         if (!is_edge_used[i]) {
            if (i > min_i)
               edge_costs[i] =
                   min(graph[number_vertices * min_i + i], edge_costs[i]);
            else
               edge_costs[i] =
                   min(graph[number_vertices * i + min_i], edge_costs[i]);
         }
      }
   }
#ifndef SEQUENTIA
#pragma omp parallel for simd reduction(+ : treeCost)
#endif
   for (int i = 0; i < number_vertices; ++i) {
      treeCost += edge_costs[i];
   }
   auto clk_end = std::chrono::high_resolution_clock::now();
   auto distance_time = std::chrono::duration_cast<std::chrono::microseconds>(
       clk_stop - clk_start);
   auto MST_time = std::chrono::duration_cast<std::chrono::microseconds>(
       clk_end - clk_stop);
   cout << treeCost << endl;
   printf("Distance time: %ld, MST time: %ld\n", distance_time.count(),
          MST_time.count());
   free(graph);
   graph = NULL;

   writeCost(treeCost, programArguments.mOutputFilePath);
}

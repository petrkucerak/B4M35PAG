#include <cstring>
#include <iostream>
#include <limits.h>
#include <omp.h>
#include <queue>
#include <vector>

#include "Utils.h"

#define TABLE(x, y) *(table + (x) + ((y) * x_size))
#define GRAPH(x, y) *(graph + (x) + ((y) * number_vertices))
#define P_Q priority_queue<Edge, vector<Edge>, CompareEdge>
#define MIN(X, Y, Z)                                                           \
   (((X) < (Y)) ? ((X) < (Z) ? (X) : (Z)) : ((Y) < (Z) ? (Y) : (Z)))

using namespace std;

struct Edge {
   int target;
   int64_t value;
   Edge(int target, int64_t value) : target(target), value(value) {}
};
struct CompareEdge {
   bool operator()(Edge const &e1, Edge const &e2)
   {
      return e1.value > e2.value;
   }
};

struct Edge_Raw {
   int target;
   int value;
   Edge_Raw(int target, int value) : target(target), value(value) {}
};

int getLevenshteinDistance(vector<vector<int>> records, int A, int B)
{
   const int x_size = records[A].size() + 1;
   const int y_size = records[B].size() + 1;
   int *table = (int *)malloc(sizeof(int) * x_size * y_size);

   for (int i = 0; i < x_size; ++i)
      TABLE(i, 0) = i;
   for (int i = 0; i < y_size; ++i)
      TABLE(0, i) = i;

   for (int y = 1; y < y_size; ++y) {
      for (int x = 1; x < x_size; ++x) {
         int substitution_cost = 1;
         if (records[A][x - 1] == records[B][y - 1]) {
            substitution_cost = 0;
         }
         TABLE(x, y) =
             MIN(TABLE(x - 1, y) + 1,                    // deletion
                 TABLE(x, y - 1) + 1,                    // insertion
                 TABLE(x - 1, y - 1) + substitution_cost // substitution
             );
      }
   }

   // cout << endl;
   // printTable(x_size, y_size, table);

   int ret = TABLE(x_size - 1, y_size - 1);
   free(table);
   table = NULL;
   return ret;
}

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

   // The input records, e.g., records[0] is the first record in the input file.
   vector<vector<int>> records = readRecords(programArguments.mInputFilePath);
   // vector<vector<int>> records = {{1, 2, 5, 5, 5, 5, 3, 2}, {1, 4, 3, 2}};

   const int number_vertices = records.size();
   int *graph = (int *)malloc(sizeof(int) * number_vertices * number_vertices);
   for (auto i = 0; i < number_vertices; ++i) {
      for (auto j = 0; j < number_vertices; ++j)
         GRAPH(i, j) = 0;
   }

   // #pragma omp parallel for shared(graph)
   for (int i = 0; i < records.size(); ++i) {
      // #pragma omp parallel for shared(graph, i)
      for (int j = i + 1; j < records.size(); ++j) {

         int cost = getLevenshteinDistance(records, i, j);

         // #pragma omp critical
         {
            GRAPH(i, j) = cost;
            // GRAPH(j, i) = cost; // TODO: allocate more effectly
         }
      }
   }

   printGraph(graph, number_vertices);

   // distances graph
   int treeCost = 0;

   vector<bool> is_vertices_used(number_vertices);
   vector<int> costs(number_vertices);

   is_vertices_used[0] = true;
   costs[0] = 0;

   int added_edges = 0;

   for (int i = 1; i < number_vertices; ++i) {
      costs[i] = graph[i];
   }

   free(graph);
   graph = NULL;

   writeCost(treeCost, programArguments.mOutputFilePath);
}

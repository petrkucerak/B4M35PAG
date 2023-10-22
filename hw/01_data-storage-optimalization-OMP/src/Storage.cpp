#include <cstring>
#include <iostream>
#include <limits.h>
#include <omp.h>
#include <queue>
#include <vector>

#include "Utils.h"

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

int getLevenshteinDistanceFast(vector<vector<int>> records, int A, int B)
{
   const int x_size = records[A].size() + 1;
   const int y_size = records[B].size() + 1;
   int *table = new int[x_size * 2];

   for (int x = 0; x < x_size; ++x)
      table[x] = x;

   int line = 1;
   for (int y = 0; y < y_size - 1; ++y) {
      table[0 + (line * x_size)] = y + 1;

      for (int x = 0; x < x_size - 1; ++x) {
         int del_cost, ins_cost, sub_cost;

         int sub_parameter = 1;
         // compare letter
         if (records[A][x] == records[B][y]) {
            sub_parameter = 0;
         }
         // new values are in bottom row
         if (line == 1) {
            del_cost = table[x + 1] + 1;
            ins_cost = table[x + x_size] + 1;
            sub_cost = table[x] + sub_parameter;
            table[x_size + x + 1] = MIN(del_cost, ins_cost, sub_cost);
         } else {
            // new valuesa re in top row
            del_cost = table[x + 1 + x_size] + 1;
            ins_cost = table[x] + 1;
            sub_cost = table[x + x_size] + sub_parameter;
            table[x + 1] = MIN(del_cost, ins_cost, sub_cost);
         }
      }
      if (line == 1)
         line = 0;
      else
         line = 1;
   }

   int ret = table[((1 - line) * x_size) + x_size - 1];
   delete[] table;

   return ret;
}

void printGraph(vector<vector<int>> graph)
{
   for (auto i = 0; i < graph.size(); ++i) {
      for (auto j = 0; j < graph[i].size(); ++j)
         printf("%2d ", graph[i][j]);
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
   vector<vector<int>> graph(number_vertices, vector<int>(number_vertices));

   // #pragma omp parallel for shared(graph)
   for (int i = 0; i < records.size(); ++i) {
      // #pragma omp parallel for shared(graph, i)
      for (int j = i + 1; j < records.size(); ++j) {

         int cost = getLevenshteinDistanceFast(records, i, j);

         // #pragma omp critical
         {
            graph[i][j] = cost;
            // graph[j][i] = cost; // TODO: allocate more effectly
         }
      }
   }

   printGraph(graph);

   // distances graph
   int treeCost = 0;

   vector<bool> vertices(number_vertices);
   for (auto i : vertices)
      i = false;
   vertices[0] = true;
   int search_size = 0;
   for (auto i = 1; i < number_vertices - 1; ++i) {
      // find next target
      int min = INT_MAX;
      // TODO: continue here
   }
   // cout << "Number of records: " << records.size() << endl;
   // cout << "Max threads count: " << omp_get_max_threads() << endl;

   // cout << treeCost << endl;
   // delete graph;

   writeCost(treeCost, programArguments.mOutputFilePath);
}

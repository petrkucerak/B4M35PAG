#include <cstring>
#include <iostream>
#include <omp.h>
#include <queue>
#include <vector>

#include "Utils.h"

#define P_Q priority_queue<Edge, vector<Edge>, CompareEdge>
#define TABLE(x, y) *(table + (x) + ((y) * x_size))
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

void printTable(int x_size, int y_size, int *table)
{
   for (int y = 0; y < y_size; ++y) {
      for (int x = 0; x < x_size; ++x) {
         printf("%2d ", TABLE(x, y));
      }
      printf("\n");
   }
}

int getLevenshteinDistance(vector<vector<int>> records, int A, int B)
{
   const int x_size = records[A].size() + 1;
   const int y_size = records[B].size() + 1;
   int *table = new int[x_size * y_size];

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
   delete table;
   return ret;
}

int main(int argc, char *argv[])
{
   auto programArguments = ProgramArguments::Parse(argc, argv);

   // The input records, e.g., records[0] is the first record in the input file.
   vector<vector<int>> records = readRecords(programArguments.mInputFilePath);
   // vector<vector<int>> records = {{1, 2, 5, 5, 5, 5, 3, 2}, {1, 4, 3, 2}};

   vector<Edge_Raw> *graph = new vector<Edge_Raw>[records.size()];
   bool v[records.size()];

#pragma omp parallel for
   for (int i = 0; i < records.size(); ++i) {
      v[i] = false;
      for (int j = i + 1; j < records.size(); ++j) {

         int cost = getLevenshteinDistance(records, i, j);

#pragma omp critical
         {
            // cout << "Thread num " << omp_get_thread_num() << endl;
            graph[i].push_back(Edge_Raw(j, cost));
            graph[j].push_back(Edge_Raw(i, cost));
         }
      }
   }

   // distances graph
   int treeCost = 0;

   int added_verticies = 1;
   P_Q *q = new P_Q;
   v[0] = true;
   // cout << records.size() << endl;
   // find neighbours
   for (int i = 0; i < graph[0].size(); ++i) {
      q->push(Edge(graph[0][i].target, graph[0][i].value));
   }

   while (added_verticies != records.size()) {
      // cout << "NOW" << endl;
      int vertex_id = q->top().target;
      if (!v[vertex_id]) {

         v[vertex_id] = true;
         treeCost += q->top().value;
         ++added_verticies;
         q->pop();

         // find neighbours
         for (int i = 0; i < graph[vertex_id].size(); ++i) {
            q->push(
                Edge(graph[vertex_id][i].target, graph[vertex_id][i].value));
         }

      } else {
         q->pop();
      }
   }

   cout << "Max threads count: " << omp_get_max_threads() << endl;

   cout << treeCost << endl;
   // delete graph;
   delete q;
   writeCost(treeCost, programArguments.mOutputFilePath);
}

#include <algorithm>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <vector>

using namespace std;

#define TASK tasks[depth]

struct Task {
   int task_id;
   int process_time;
   int release_time;
   int deadline;
};

void taskSwap(Task &a, Task &b)
{
   Task temp = a;
   a = b;
   b = temp;
}

bool catchDeadline(vector<Task> tasks, int start_time, int depth)
{
   while (depth < tasks.size()) {
      if (TASK.process_time + start_time > TASK.deadline)
         return false;
      ++depth;
   }
   return true;
}

void bratleyAlgorithm(vector<Task> &tasks, vector<int> order, int start_time,
                      int &best_time, int depth)
{
   if (depth == tasks.size()) {
      cout << "Last element: " << TASK.task_id << endl;
      cout << "with time: " << start_time << endl << endl;
   }
   for (int i = 0; i < tasks.size() - depth; ++i) {
      // swap
      taskSwap(tasks[depth], tasks[depth + i]);

      // 1. CONDITION: Missing deadline
      if (!catchDeadline(tasks, start_time, depth)) {
         taskSwap(tasks[depth + i], tasks[depth]);
         continue;
      }

      // get end time
      const int end_time =
          max(TASK.release_time, start_time) + TASK.process_time;

      // run the new step of bratley algorithm
      bratleyAlgorithm(tasks, order, end_time, best_time, depth + 1);

      // swap back
      taskSwap(tasks[depth + i], tasks[depth]);
   }
}

int main(int argc, char **argv)
{
   // Initialize MPI
   MPI_Init(&argc, &argv);
   int world_size, rank;
   MPI_Comm_size(MPI_COMM_WORLD, &world_size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   if (!rank) {
      // Open the file
      ifstream inputFile(argv[1]);
      if (!inputFile.is_open()) {
         fprintf(stderr, "Error opening the file: %s\n", argv[1]);
         goto FIASKO;
      }

      int task_count;
      if (!(inputFile >> task_count)) {
         fprintf(stderr, "Can't load the task count from file.\n");
         goto FIASKO;
      }

      vector<Task> tasks;
      vector<int> order(task_count);
      tasks.reserve(task_count);
      int best_time = INT32_MAX;

      // Open the output file
      ofstream outputFile(argv[2]);
      if (!outputFile.is_open()) {
         fprintf(stderr, "Error opening the output file: %s\n", argv[2]);
         goto FIASKO;
      }

      // Load the data from the file
      for (int i = 0; i < task_count; ++i) {
         int process_time, release_time, deadline;
         if (!(inputFile >> process_time >> release_time >> deadline)) {
            fprintf(stderr, "Can't load a task from file.\n");
            goto FIASKO;
         }
         // if deadline - process_time < release_time cause error
         if (process_time + release_time > deadline) {
            outputFile << "-1" << endl;
            goto END;
         }
         tasks.push_back({i, process_time, release_time, deadline});
      }

      // Sort tasks based on release times
      // TODO: sort tasks vector?
      // sort(tasks.begin(), tasks.end(), [](const Task &a, const Task &b) {
      //    return a.release_time < b.release_time;
      // });

      bratleyAlgorithm(tasks, order, 0, best_time, 0);
   }
END:
   MPI_Finalize();
   return 0;
FIASKO:
   MPI_Finalize();
   return 1;
}

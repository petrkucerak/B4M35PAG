#include <algorithm>
#include <climits>
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

vector<int> best_order;

bool compareTasks(const Task &task1, const Task &task2)
{
   // First, compare by deadline
   if (task1.deadline < task2.deadline) {
      return true;
   } else if (task1.deadline > task2.deadline) {
      return false;
   } else {
      // If deadline is the same, compare by release_time
      return task1.release_time < task2.release_time;
   }
}

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

bool isReleaseLater(vector<Task> &tasks, int start_time, int depth)
{
   while (depth < tasks.size()) {
      if (TASK.release_time < start_time)
         return false;
      ++depth;
   }
   return true;
}

bool bratleyAlgorithm(vector<Task> &tasks, vector<int> &order, int start_time,
                      int depth, int &best_time)
{
   bool skip_parents = false;
   if (depth == tasks.size()) {
      if (start_time < best_time) {
         best_time = start_time;
         best_order = order;
      }
      return skip_parents;
   }

   // 1. CONDITION: Missing deadline
   if (!catchDeadline(tasks, start_time, depth)) {
      // cout << "Missing deadline" << endl;
      return skip_parents;
   }
   // 2. CONDITION: Bound on the solution
   int count_process_time = 0;
   int sooner_release_time = 0;
   int best_release_Time = 0;
   int peters_magic = 0;
   int history_sum = 0;
   for (int i = depth; i < tasks.size(); ++i) {
      peters_magic += tasks[i].process_time;
      if (tasks[i].release_time > start_time) {
         peters_magic +=
             (tasks[i].release_time - start_time - count_process_time);
      } else {
         history_sum += tasks[i].process_time;
      }
      count_process_time += tasks[i].process_time;
      sooner_release_time = tasks[i].release_time < sooner_release_time
                                ? tasks[i].release_time
                                : sooner_release_time;
      best_release_Time = tasks[i].release_time > best_release_Time
                              ? tasks[i].release_time
                              : best_release_Time;
   }
   if (max(start_time, sooner_release_time) + count_process_time >= best_time)
      return skip_parents;

   // 3. CONDITION: Decomposition
   if (sooner_release_time >= start_time)
      skip_parents = true;

   // peter's smart heuristic
   if ((history_sum < best_release_Time - start_time) &&
       start_time + peters_magic >= best_time)
      return skip_parents;

   bool is_best_tmp = false;
   for (int i = depth; i < tasks.size(); ++i) {
      // swap
      taskSwap(tasks[depth], tasks[i]);

      order[depth] = TASK.task_id;

      // get end time
      const int end_time =
          max(TASK.release_time, start_time) + TASK.process_time;

      // run the new step of bratley algorithm
      skip_parents =
          bratleyAlgorithm(tasks, order, end_time, depth + 1, best_time);

      // swap back
      taskSwap(tasks[i], tasks[depth]);
      if (skip_parents) {
         return skip_parents;
      }
   }
   return skip_parents;
}

int main(int argc, char **argv)
{
   // Initialize MPI
   MPI_Init(&argc, &argv);
   int world_size, rank;
   MPI_Comm_size(MPI_COMM_WORLD, &world_size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Barrier(MPI_COMM_WORLD);
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
      int best_time = INT_MAX;

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
            cout << "-1" << endl;
            goto END;
         }
         tasks.push_back({i, process_time, release_time, deadline});
      }

      // Sort tasks based on release times
      sort(tasks.begin(), tasks.end(), compareTasks);

      bratleyAlgorithm(tasks, order, 0, 0, best_time);
      if (!best_order.empty()) {
         for (auto &task : best_order)
            outputFile << task << endl;
         for (auto &task : best_order)
            cout << task << endl;
      } else {
         outputFile << "-1" << endl;
         cout << "-1" << endl;
      }
   }
END:
   MPI_Finalize();
   return 0;
FIASKO:
   MPI_Finalize();
   return 1;
}

#include <algorithm>
#include <climits>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <vector>

using namespace std;

#define TASK tasks[depth]

struct Task {
   unsigned int task_id;
   unsigned int process_time;
   unsigned int release_time;
   unsigned int deadline;
};

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

bool catchDeadline(vector<Task> tasks, unsigned int start_time,
                   unsigned int depth)
{
   while (depth < tasks.size()) {
      if (TASK.process_time + start_time > TASK.deadline)
         return false;
      ++depth;
   }
   return true;
}

bool isAlwaysSlower(vector<Task> &tasks, unsigned int start_time,
                    unsigned int depth, unsigned int best_time)
{
   while (depth < tasks.size()) {
      start_time += TASK.process_time;
      ++depth;
   }
   return best_time < start_time;
}

bool isReleaseLater(vector<Task> &tasks, unsigned int start_time,
                    unsigned int depth)
{
   while (depth < tasks.size()) {
      if (TASK.release_time < start_time)
         return false;
      ++depth;
   }
   return true;
}

void bratleyAlgorithm(vector<Task> &tasks, vector<int> &order,
                      unsigned int start_time, unsigned int &best_time,
                      unsigned int depth, bool &is_best, bool &skip_parents)
{
   // 1. CONDITION: Missing deadline
   if (!catchDeadline(tasks, start_time, depth)) {
      // cout << "Missing deadline" << endl;
      return;
   }

   // 2. CONDITION: Bound on the solution
   if (is_best && isAlwaysSlower(tasks, start_time, depth, best_time)) {
      cout << "2. condition" << endl;
      return;
   }
   bool is_best_tmp = false;
   for (int i = 0; i < tasks.size() - depth; ++i) {
      // swap
      taskSwap(tasks[depth], tasks[depth + i]);

      // get end time
      const int end_time =
          max(TASK.release_time, start_time) + TASK.process_time;

      if (tasks.size() - depth == 1) {
         if (end_time < best_time) {
            // cout << "Last element: " << TASK.task_id << endl;
            // cout << "with time: " << end_time << endl << endl;
            best_time = end_time;
            is_best_tmp = true;
            order[TASK.task_id] = max(TASK.release_time, start_time);
         }
         taskSwap(tasks[depth], tasks[depth + i]);
         continue;
         ;
      }

      // run the new step of bratley algorithm
      bratleyAlgorithm(tasks, order, end_time, best_time, depth + 1, is_best,
                       skip_parents);
      if (is_best) {
         is_best_tmp = true;
         order[TASK.task_id] = max(TASK.release_time, start_time);
      }
      is_best = false;

      // swap back
      taskSwap(tasks[depth + i], tasks[depth]);
      if (skip_parents)
         break;
   }
   // 3. CONDITION: Decomposition
   if (!skip_parents && isReleaseLater(tasks, start_time, depth)) {
      cout << "3. condition" << endl;
      skip_parents = true;
   }
   is_best = is_best_tmp;
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
      unsigned int best_time = UINT32_MAX;

      // Open the output file
      ofstream outputFile(argv[2]);
      if (!outputFile.is_open()) {
         fprintf(stderr, "Error opening the output file: %s\n", argv[2]);
         goto FIASKO;
      }

      // Load the data from the file
      for (unsigned int i = 0; i < task_count; ++i) {
         unsigned int process_time, release_time, deadline;
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

      bool is_best = false;
      bool skip_parents = false;
      bratleyAlgorithm(tasks, order, 0, best_time, 0, is_best, skip_parents);
      if (is_best) {
         for (auto &task : order)
            outputFile << task << endl;
         for (auto &task : order)
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

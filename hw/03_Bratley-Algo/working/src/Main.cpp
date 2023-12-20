#include <algorithm>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <vector>

#define UNDEFINED -1
#define LAST_TASK tasks[tasks.size() - 1]

using namespace std;

struct Task {
   int task_id;
   int process_time;
   int release_time;
   int deadline;
};

bool catchDeadline(const vector<Task> &tasks, int start_time, int n)
{
   for (int i = 0; i < tasks.size() - n; ++i)
      if (tasks[i].process_time + start_time > tasks[i].deadline)
         return false;
   return true;
}

bool isAlwaysSlower(const vector<Task> &tasks, int start_time, int best_time,
                    int start_task_id)
{

   int potencial_best_time = start_time;
   for (int i = start_task_id; i < tasks.size(); ++i)
      potencial_best_time += tasks[i].process_time;

   return best_time < potencial_best_time;
}

bool isReleaseLater(const vector<Task> &tasks, int start_time,
                    int start_task_id)
{
   bool ret = true;
   for (int i = start_task_id; i < tasks.size(); ++i)
      if (tasks[i].release_time < start_time)
         return false;
   return ret;
}
void swap(int &a, int &b)
{
   int temp = a;
   a = b;
   b = temp;
}

// TODO: use swap
bool bratleyAlgorithm(vector<Task> &tasks, vector<int> &order, int start_time,
                      int &best_time, bool &best_base, int n)
{
   // 1. CONDITION: Missing deadline
   // if (!catchDeadline(tasks, start_time, n))
   //    return false;

   if (n == 0) {
      // cout << "Find solution with time: " << start_time << endl;

      // for (auto &task : tasks)
      //    cout << task.task_id << " ";
      // cout << endl;

      // if the task is new best time
      if (start_time < best_time) {
         best_time = start_time;
         order[LAST_TASK.task_id] = start_time - LAST_TASK.process_time;
         return true;
      } else {
         return false;
      }
   }

   bool ret = false;
   const int task_id = tasks.size() - n;
   for (int i = 0; i < n; ++i) {
      swap(tasks[task_id], tasks[task_id + i]); // TODO: replace by vector swap

      // 2. CONDITION: Bound on the solution
      if (ret && isAlwaysSlower(tasks, start_time, best_time, task_id)) {
         swap(tasks[task_id + i], tasks[task_id]);
         continue;
      }
      bool status =
          bratleyAlgorithm(tasks, order,
                           max(start_time, tasks[task_id].release_time) +
                               tasks[task_id].process_time,
                           best_time, best_base, n - 1);
      if (status) {
         order[tasks[task_id].task_id] =
             max(start_time, tasks[task_id].release_time);
         ret = true;
      }
      swap(tasks[task_id + i], tasks[task_id]);
      if (best_base)
         break;
   }
   // 3. CONDITION: Decomposition
   if (isReleaseLater(tasks, start_time, task_id)) {
      best_base = true;
   }
   return ret;
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
      sort(tasks.begin(), tasks.end(), [](const Task &a, const Task &b) {
         return a.release_time < b.release_time;
      });

      bool best_base = false;
      if (!bratleyAlgorithm(tasks, order, 0, best_time, best_base, task_count))
         outputFile << "-1" << endl;
      else
         for (auto &task : order)
            outputFile << task << endl;

      for (auto &task : order)
         cout << task << endl;
   }
END:
   MPI_Finalize();
   return 0;
FIASKO:
   MPI_Finalize();
   return 1;
}

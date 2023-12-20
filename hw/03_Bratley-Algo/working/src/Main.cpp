#include <algorithm>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <vector>

#define UNDEFINED -1
#define LAST_TASK tasks[0]

using namespace std;

struct Task {
   int task_id;
   int process_time;
   int release_time;
   int deadline;
};

bool catchDeadline(vector<Task> tasks, int start_time)
{
   for (auto &task : tasks) {
      if (task.process_time + start_time > task.deadline)
         return false;
   }
   return true;
}

bool isAlwaysSlower(vector<Task> tasks, int start_time, int best_time)
{

   int potencial_best_time = start_time;
   for (auto &task : tasks) {
      potencial_best_time += task.process_time;
   }
   return best_time < potencial_best_time ? true : false;
}

bool bratleyAlgorithm(vector<Task> tasks, vector<int> &order, int start_time,
                      int &best_time)
{
   // for (auto &task : tasks)
   //    printf("[%d %d %d] ", task.process_time, task.release_time,
   //           task.deadline);
   // printf("\n");

   // 1. CONDITION: Missing deadline
   if (!catchDeadline(tasks, start_time))
      return false;

   // 3. condition
   if (tasks.size() == 1) {
      int time =
          max(start_time, LAST_TASK.release_time) + LAST_TASK.process_time;
      // cout << "Find solution with time: " << time << endl;

      // if the task is new best time
      if (time < best_time) {
         best_time = time;
         order[LAST_TASK.task_id] = max(start_time, LAST_TASK.release_time);
         return true;
      } else {
         return false;
      }
   }
   bool ret = false;
   for (int i = 0; i < tasks.size(); ++i) {
      vector<Task> new_tasks = tasks;
      new_tasks.erase(new_tasks.begin() + i);

      // 2. CONDITION: Bound on the solution
      if (ret) {
         if (isAlwaysSlower(new_tasks, start_time, best_time))
            continue;
      }

      bool status = bratleyAlgorithm(new_tasks, order,
                                     max(start_time, tasks[i].release_time) +
                                         tasks[i].process_time,
                                     best_time);
      if (status) {
         order[tasks[i].task_id] = max(start_time, tasks[i].release_time);
         ret = true;
      }
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

      // TODO: sort tasks vector

      if (!bratleyAlgorithm(tasks, order, 0, best_time))
         outputFile << "-1" << endl;
      else
         for (auto &task : order)
            outputFile << task << endl;
   }
END:
   MPI_Finalize();
   return 0;
FIASKO:
   MPI_Finalize();
   return 1;
}

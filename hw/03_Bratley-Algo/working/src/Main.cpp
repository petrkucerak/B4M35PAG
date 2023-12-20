#include <algorithm>
#include <iostream>
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

bool bratleyAlgorithm(vector<Task> tasks, vector<int> &order, int start_time,
                      int &best_time)
{
   // for (auto &task : tasks)
   //    printf("[%d %d %d] ", task.process_time, task.release_time,
   //           task.deadline);
   // printf("\n");

   // 1. Missing deadline
   if (!catchDeadline(tasks, start_time))
      return false;
   // 2. condition
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

int main(int argc, char const *argv[])
{
   int task_count;
   if (scanf("%d\n", &task_count) != 1) {
      fprintf(stderr, "Can't load the task count\n");
      exit(EXIT_FAILURE);
   }

   vector<Task> tasks;
   vector<int> order(task_count);
   tasks.reserve(task_count);
   int best_time = INT32_MAX;

   // Load the data
   for (int i = 0; i < task_count; ++i) {
      int process_time, release_time, deadline;
      if (scanf("%d %d %d\n", &process_time, &release_time, &deadline) != 3) {
         fprintf(stderr, "Can't load a task\n");
         exit(EXIT_FAILURE);
      }
      // if deadline - process_time < release_time cause error
      if (process_time + release_time > deadline) {
         cout << "-1" << endl;
         return 0;
      }
      tasks.push_back({i, process_time, release_time, deadline});
   }

   if (!bratleyAlgorithm(tasks, order, 0, best_time))
      printf("-1\n");
   else
      for (auto &task : order)
         printf("%d\n", task);

   return 0;
}

#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

struct Task {
   int process_time;
   int release_time;
   int deadline;
};

void bratleyAlgorithm(vector<Task> tasks)
{
   for (int i = 0; i < tasks.size() - 1; ++i) {
      vector<Task> new_tasks = tasks;
      new_tasks.erase(new_tasks.begin() + i);
      bratleyAlgorithm(new_tasks);
   }
}

int main(int argc, char const *argv[])
{
   int task_count;
   if (scanf("%d\n", &task_count) != 1) {
      fprintf(stderr, "Can't load the task count\n");
      exit(EXIT_FAILURE);
   }

   vector<Task> tasks;
   tasks.reserve(task_count);

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
      tasks.push_back({process_time, release_time, deadline});
   }

   bratleyAlgorithm(tasks);

   return 0;
}

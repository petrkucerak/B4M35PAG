#include <deque>
#include <iostream>
#include <mpi.h>
#include <queue>
#include <vector>

#define UNDEFINED -1

using namespace std;

struct Task {
   int process_time;
   int release_time;
   int deadline;
};

struct History {
   int task_id;
   int start_time;
};

/**
 * @brief This struct is using in the dequeue
 *
 */
struct Node {
   int task_id;
   int timestamp; /* The start of task */
   int depth;
   vector<History> order;
};

const void print_tasks(vector<Task> &tasks)
{
   printf("\n");
   for (auto task : tasks) {
      printf("%d %d %d\n", task.process_time, task.release_time, task.deadline);
   }
}

void addChildrens(deque<Node> &deque, vector<Task> &tasks, int depth,
                  int task_id, int timestamp, vector<History> order)
{
   int already_added = 0;
   int new_children_count = tasks.size() - depth;
   while (already_added <= new_children_count) {
      task_id != tasks.size() - 1 ? ++task_id : task_id = 0;
      order.push_back({task_id, timestamp});
      deque.push_back({task_id, timestamp, depth + 1, order});
      ++already_added;
   }
}

bool containMissingDeadline(vector<Task> &tasks, int depth, int task_id,
                            int timestamp)
{
   int already_added = 0;
   int new_children_count = tasks.size() - depth;
   while (already_added <= new_children_count) {
      task_id != tasks.size() - 1 ? ++task_id : task_id = 0;
      if (tasks[task_id].process_time + timestamp > tasks[task_id].deadline) {
         printf("\033[1;31m%d + %d > %d\033[0m\n", tasks[task_id].process_time,
                timestamp, tasks[task_id].deadline);
         printf("\n");
         return true;
      } else {
         printf("\033[1;33m%d + %d > %d\033[0m\n", tasks[task_id].process_time,
                timestamp, tasks[task_id].deadline);
      }
      ++already_added;
   }
   printf("\033[1;32mOK\033[0m\n");
   return false;
}

int main(int argc, char **argv)
{
   // Initialize MPI
   MPI_Init(&argc, &argv);
   int world_size, rank;
   MPI_Comm_size(MPI_COMM_WORLD, &world_size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);

   if (!rank) {
      // Load the metadata
      bool exits_solution = false;
      int best_solution = INT32_MAX;
      int task_count;
      if (scanf("%d\n", &task_count) != 1) {
         fprintf(stderr, "Can't load the task count\n");
         MPI_Finalize();
         exit(EXIT_FAILURE);
      }

      vector<Task> tasks;
      vector<int> final_order(task_count);
      deque<Node> deque;
      tasks.reserve(task_count);

      // Load the data
      for (int i = 0; i < task_count; ++i) {
         int process_time, release_time, deadline;
         if (scanf("%d %d %d\n", &process_time, &release_time, &deadline) !=
             3) {
            fprintf(stderr, "Can't load a task\n");
            MPI_Finalize();
            exit(EXIT_FAILURE);
         }
         // if deadline - process_time < release_time cause error
         if (process_time + release_time > deadline)
            goto INCORRECT;
         tasks.push_back({process_time, release_time, deadline});
      }

      // print_tasks(tasks);
      // Add first level to the end of deque
      for (int i = 0; i < tasks.size(); ++i) {
         vector<History> order;
         order.push_back({i, 0});
         deque.push_back({i, 0, 1, order});
      }

      while (!deque.empty()) {
         // Get assignment from front of deque
         Node node = deque.front();
         Task task = tasks[node.task_id];
         deque.pop_front();

         // 1. test practicability of this element
         int timestamp = max(node.timestamp, task.release_time) +
                         task.process_time; // end of current task

         // cout << node.task_id << "(" << timestamp << ")[" << node.depth <<
         // "]"
         //      << endl;

         // 1. Missed deadline for all childrens
         if (timestamp > task.deadline)
            continue;

         // if (containMissingDeadline(tasks, node.depth, node.task_id,
         // timestamp))
         //    continue;

         // Detect the end of timestamp
         if (node.depth == task_count) {
            exits_solution = true;
            if (timestamp < best_solution) {
               best_solution = timestamp;
               for (auto order : node.order) {
                  final_order[order.task_id] = order.start_time;
               }
            }
         }

         // 2. add its children
         addChildrens(deque, tasks, node.depth, node.task_id, timestamp,
                      node.order);
      }

      if (!exits_solution) {
      INCORRECT:
         cout << "-1" << endl;
      } else {
         cout << "The best solution is process with time: " << best_solution
              << endl;
         for (auto order : final_order)
            printf("%d\n", order);
      }
   }

   // cout << "Hello wordl from node " << rank << "!" << endl;

   MPI_Finalize();
   return 0;
}
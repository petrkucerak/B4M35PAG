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

/**
 * @brief This struct is using in the dequeue
 *
 */
struct Node {
   int task_id;
   int timestamp; /* The start of task */
   int depth;
};

const void print_tasks(vector<Task> &tasks)
{
   printf("\n");
   for (auto task : tasks) {
      printf("%d %d %d\n", task.process_time, task.release_time, task.deadline);
   }
}

void addChildrens(deque<Node> &deque, vector<Task> &tasks, int depth,
                  int task_id, int timestamp)
{
   int already_added = 0;
   int new_children_count = tasks.size() - depth;
   while (already_added < new_children_count) {
      task_id != tasks.size() - 1 ? ++task_id : task_id = 0;
      deque.push_back({task_id, timestamp, depth + 1});
      ++already_added;
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
      // Load the metadata
      bool exits_solution = false;
      int task_count;
      if (scanf("%d\n", &task_count) != 1) {
         fprintf(stderr, "Can't load the task count\n");
         MPI_Finalize();
         exit(EXIT_FAILURE);
      }

      vector<Task> tasks;
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
            cout << "-1" << endl;
         break;
         tasks.push_back({process_time, release_time, deadline});
      }

      // print_tasks(tasks);

      deque<Node> deque;
      // Add first level to the end of deque
      for (int i = 0; i < tasks.size(); ++i) {
         deque.push_back({i, 0, 1});
      }

      while (!deque.empty()) {
         // Get assignment from front of deque
         Node node = deque.front();
         Task task = tasks[node.task_id];
         deque.pop_front();

         // 1. test practicability of this element
         int timestamp =
             (node.timestamp >= task.release_time ? node.timestamp
                                                  : task.release_time) +
             task.process_time; // end of current task
         // cout << node.task_id << "(" << timestamp << ")" << endl;
         // Missed deadline
         if (timestamp > task.deadline)
            continue;

         // Detect the end of timestamp
         if (node.depth == task_count) {
            cout << "TASK COUNT: " << task_count << endl;
            cout << "NOW" << endl;
            cout << "T: " << node.task_id << " D: " << node.depth << endl;
            exits_solution = true;
         }

         // 2. add its children
         addChildrens(deque, tasks, node.depth, node.task_id, timestamp);
      }

      if (!exits_solution)
         cout << "-1" << endl;
   }

   // cout << "Hello wordl from node " << rank << "!" << endl;

   MPI_Finalize();
   return 0;
}
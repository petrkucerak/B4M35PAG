#include <iostream>
#include <mpi.h>
#include <vector>

#define UNDEFINED -1

using namespace std;

struct Task {
   int process_time;
   int release_time;
   int deadline;
};

const void print_tasks(vector<Task> &tasks)
{
   printf("\n");
   for (auto task : tasks) {
      printf("%d %d %d\n", task.process_time, task.release_time, task.deadline);
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
         tasks.push_back({process_time, release_time, deadline});
      }

      print_tasks(tasks);
   }

   cout << "Hello wordl from node " << rank << "!" << endl;

   MPI_Finalize();
   return 0;
}
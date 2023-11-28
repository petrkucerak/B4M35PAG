#pragma once

#include "DebuggingTools.h"
#include "mpi.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <unistd.h>
#include <vector>

using namespace std;

class Master
{
   private:
   int task_iterator;
   int num_slaves;
   int num_active_slaves;
   int num_task;
   bool end_work;
   int my_rank;
   vector<int> tasks;
   vector<bool> activated_connections;
   vector<char> incoming_messages;
   vector<MPI_Status> slave_stats;
   vector<MPI_Request> slave_reqs;
   vector<bool> active_slaves;

   public:
   Master(int size);

   void prepare_data();

   void terminate_slaves();

   void activate_connections();

   void work();
};

Master::Master(int size)
{
   my_rank = 0;
   num_slaves = size - 1;
   num_active_slaves = num_slaves;
   end_work = false;
   activated_connections = vector<bool>(num_slaves, false);
   active_slaves = vector<bool>(num_slaves, true);
   incoming_messages = vector<char>(num_slaves);
   slave_reqs = vector<MPI_Request>(num_slaves);
   slave_stats = vector<MPI_Status>(num_slaves);
}

// Master must dynamically receive incoming messages
void Master::activate_connections()
{
   for (int slave = 0; slave < num_slaves; slave++) {
      int slave_rank = slave + 1;
      if (!active_slaves[slave]) {
         continue;
      }
      if (!activated_connections[slave]) {
         // printf("Master activating connection with slave %d\n", slave_rank);
         MPI_Irecv(&incoming_messages[slave], 1, MPI_CHAR, slave_rank, 0,
                   MPI_COMM_WORLD, &slave_reqs[slave]);
         activated_connections[slave] = true;
      }
   }
}

void Master::prepare_data()
{
   int task_time, final_task;

   // Get number of tasks and final task position
   cout << "Enter number of tasks: ";
   cin >> num_task;
   cout << "Enter final task index: ";
   cin >> final_task;

   // Clip values to prevent segfaults
   num_task = max(1, num_task);
   final_task = max(0, final_task);
   final_task = min(final_task, num_task - 1);

   // Set task iterator to 0
   task_iterator = 0;

   // Generate some random tasks
   for (int i = 0; i < num_task; i++) {
      task_time = (rand() % 10) + 1;
      tasks.push_back(task_time);
   }
   tasks[final_task] = -1;

   // Print master tasks
   ccout(my_rank) << "Master has tasks: " << vector_to_string<int>(tasks)
                  << cendl;
}

void Master::work()
{
   while (!end_work) {
      // Update pipeline for incoming messages
      activate_connections();

      for (int slave = 0; slave < num_slaves; slave++) {
         int flag, slave_task, slave_rank;
         char slave_message, incoming_message;

         // Set correct slave rank
         slave_rank = slave + 1;

         // Test if slave send any message
         MPI_Test(&slave_reqs[slave], &flag, &slave_stats[slave]);
         if (flag == 0) {
            // Slave did not send any message
            continue;
         } else {
            incoming_message = incoming_messages[slave];

            if (incoming_message == 'E') {
               // Slave send end request -> terminate slave and end working loop
               ccout(my_rank) << "Master received END request from slave "
                              << slave_rank << cendl;
               end_work = true;
               activated_connections[slave] = false;

               // Prepare info message and task
               slave_message = 'E';
               slave_rank = slave + 1;

               // Send terminating message and decrease the number of active
               // slaves
               ccout(my_rank)
                   << "Master TERMINATING slave " << slave_rank << cendl;
               MPI_Send(&slave_message, 1, MPI_CHAR, slave_rank, 0,
                        MPI_COMM_WORLD);
               active_slaves[slave] = false;
               num_active_slaves--;
            } else {
               // Slave send work request -> send him new task

               // Slave send me a message -> connection is deactivated
               activated_connections[slave] = false;

               // Prepare info message and task
               slave_message = 'W';
               slave_task = tasks[task_iterator];

               // Send slave an info about work and then send new task
               ccout(my_rank) << "Master received WORK request from slave "
                              << slave_rank << cendl;
               MPI_Send(&slave_message, 1, MPI_CHAR, slave_rank, 0,
                        MPI_COMM_WORLD);
               ccout(my_rank) << "Master sending task " << slave_task
                              << " to slave " << slave_rank << cendl;
               MPI_Send(&slave_task, 1, MPI_INT, slave_rank, 0, MPI_COMM_WORLD);
               task_iterator = min(task_iterator + 1, num_task - 1);
            }
         }
      }
   }

   // Terminate all working slaves
   terminate_slaves();
}

void Master::terminate_slaves()
{
   // Working loop until all slaves are terminated
   while (num_active_slaves > 0) {
      // Update pipeline for incoming messages
      activate_connections();

      for (int slave = 0; slave < num_slaves; slave++) {
         int flag, slave_rank;
         char slave_message;

         // Set correct slave rank
         slave_rank = slave + 1;

         if (!active_slaves[slave]) {
            // Slave is terminated
            continue;
         }

         // Test if slave send any message (remember that the incoming message
         // must be received)
         MPI_Test(&slave_reqs[slave], &flag, &slave_stats[slave]);
         if (flag == 0) {
            // Slave did not send any message
            continue;
         } else {
            // Terminate slave

            // Slave send me a message -> connection is deactivated
            activated_connections[slave] = false;

            //  Prepare info message for slave
            slave_message = 'E';

            // Send terminating message and decrease the number of active slaves
            ccout(my_rank) << "Master TERMINATING slave " << slave_rank
                           << cendl;
            MPI_Send(&slave_message, 1, MPI_CHAR, slave_rank, 0,
                     MPI_COMM_WORLD);
            active_slaves[slave] = false;
            num_active_slaves--;
         }
      }
   }
   // Finish
   ccout(my_rank) << "Master TERMINATING" << cendl;
}

class Slave
{
   private:
   int task;
   int my_rank;
   char incoming_message;
   char master_message;
   bool end_work;

   MPI_Status status;

   public:
   Slave(int rank);

   void work();

   void init();
};

Slave::Slave(int rank) { my_rank = rank; }

void Slave::init()
{
   // Initialization of slave
   end_work = false;

   // Send work request to master
   master_message = 'W';
   MPI_Send(&master_message, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
}

void Slave::work()
{
   // Work loop of slave
   while (!end_work) {
      // Receive incoming message from master
      MPI_Recv(&incoming_message, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
      if (incoming_message == 'W') {
         // Master will send work -> Receive new task
         MPI_Recv(&task, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
         if (task > 0) {
            // If task is not -1 than do some work
            ccout(my_rank) << "Slave " << my_rank << " will be WORKING for "
                           << task << " seconds" << cendl;
            sleep(task);

            // Send work request to master
            master_message = 'W';
            ccout(my_rank) << "Slave " << my_rank << " sending WORK request"
                           << cendl;
            MPI_Send(&master_message, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
         } else {
            // Send end request to master
            master_message = 'E';
            ccout(my_rank) << "Slave " << my_rank << " sending END request"
                           << cendl;
            MPI_Send(&master_message, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
         }
      } else {
         // Master terminating slave -> end loop
         end_work = true;
         ccout(my_rank) << "Slave " << my_rank << " TERMINATING" << cendl;
      }
   }
}

int main(int argc, char **argv)
{
   int rank;
   int size;

   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   sleep(5);
   if (rank == 0) {
      Master master = Master(size);
      master.prepare_data();
      MPI_Barrier(MPI_COMM_WORLD);
      master.work();
   } else {
      Slave slave = Slave(rank);
      slave.init();
      MPI_Barrier(MPI_COMM_WORLD);
      slave.work();
   }
   MPI_Finalize();

   return 0;
}
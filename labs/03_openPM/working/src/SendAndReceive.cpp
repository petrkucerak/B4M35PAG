#include <iostream>
#include <mpi.h>
#include <string>

using namespace std;

int main(int argc, char **argv)
{
   MPI_Init(&argc, &argv);

   int my_rank;
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

   if (my_rank == 0) {
      // Sends message.
      string message = "IDDQD";

      MPI_Send(message.c_str(), message.length() + 1, MPI_CHAR, 1, 0,
               MPI_COMM_WORLD);

   } else if (my_rank == 1) {
      // Receives message.
      constexpr int buf_size = 256;
      char buf[buf_size];

      MPI_Status status;
      MPI_Recv(&buf, buf_size, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG,
               MPI_COMM_WORLD, &status);
      int received_size;
      MPI_Get_count(&status, MPI_CHAR, &received_size);

      cout << "Received message size: " << received_size << endl;
      cout << "Received message content: " << string(buf) << endl;
   }

   MPI_Finalize();
}

#include <iostream>
#include <mpi.h>

using namespace std;

int main(int argc, char **argv)
{
   cout << "How many processes running and what is my rank?:" << endl;

   MPI_Init(&argc, &argv);

   int my_rank;
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
   cout << "My rank " << my_rank << endl;

   if (my_rank == 0) {
      int size;
      MPI_Comm_size(MPI_COMM_WORLD, &size);

      cout << "Size " << size << endl;
   }

   MPI_Finalize();
}

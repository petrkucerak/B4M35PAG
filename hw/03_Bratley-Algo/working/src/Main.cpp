#include <iostream>
#include <mpi.h>

using namespace std;

int main(int argc, char **argv)
{
   // Initialize MPI
   MPI_Init(&argc, &argv);
   int world_size, my_rank;
   MPI_Comm_size(MPI_COMM_WORLD, &world_size);
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

   cout << "Hello wordl from node " << my_rank << "!" << endl;

   MPI_Finalize();
   return 0;
}
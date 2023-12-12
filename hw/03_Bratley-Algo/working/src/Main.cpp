#include <iostream>
#include <mpi.h>

using namespace std;

int main(int argc, char **argv)
{
   // Initialize MPI
   MPI_Init(&argc, &argv);
   int worldSize, myRank;
   MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
   MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

   cout << "Hello wordl!" << endl;

   MPI_Finalize();
   return 0;
}
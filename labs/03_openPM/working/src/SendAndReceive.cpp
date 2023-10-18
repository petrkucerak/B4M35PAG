#include <iostream>
#include <string>
#include <mpi.h>

using namespace std;

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int myRank;
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    if (myRank == 0) {
        // Sends message.
        string message = "IDDQD";

       //TODO
    } else if (myRank == 1) {
        // Receives message.
        constexpr int bufSize = 256;
        char buf[bufSize];

        //TODO

        cout << "Received message size: " << receivedSize << endl;
        cout << "Received message content: " << string(buf) << endl;
    }


    MPI_Finalize();
}

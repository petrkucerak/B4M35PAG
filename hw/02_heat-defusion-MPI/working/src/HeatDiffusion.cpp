#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <mpi.h>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;
using namespace std::chrono;

#define ACCURACY 0.00001f
#define DEFAULT_TEMPERATURE 128
#define GET_MAP(x, y) *(map + (x) + width * (y))
#define GET_MASK(x, y) *(mask + (x) + width * (y))

#define WORLD_CORRECTION (width * height / worldSize * myRank)

#define PERMANENT 1
#define LEFT_TOP 10
#define TOP 11
#define RIGHT_TOP 12
#define RIGHT 13
#define RIGHT_BOTTOM 14
#define BOTTOM 15
#define LEFT_BOTTOM 16
#define LEFT 17

// Spot with permanent temperature on coordinates [x,y].
struct Spot {
   int mX;
   int mY;
   float mTemperature;

   bool operator==(const Spot &b) const { return (mX == b.mX) && (mY == b.mY); }
};

tuple<int, int, vector<Spot>> readInstance(string instanceFileName)
{
   int width, height;
   vector<Spot> spots;
   string line;

   ifstream file(instanceFileName);
   if (file.is_open()) {
      int lineId = 0;
      while (std::getline(file, line)) {
         stringstream ss(line);
         if (lineId == 0) {
            ss >> width;
         } else if (lineId == 1) {
            ss >> height;
         } else {
            int i, j, temperature;
            ss >> i >> j >> temperature;
            spots.push_back({i, j, (float)temperature});
         }
         lineId++;
      }
      file.close();
   } else {
      throw runtime_error("It is not possible to open instance file!\n");
   }
   return make_tuple(width, height, spots);
}

void writeOutput(const int myRank, const int width, const int height,
                 const string outputFileName, float *temperatures)
{
   // Draw the output image in Netpbm format.
   ofstream file(outputFileName);
   if (file.is_open()) {
      if (myRank == 0) {
         file << "P2\n" << width << "\n" << height << "\n" << 255 << "\n";
         for (unsigned int i = 0; i < width * height; ++i) {
            file << (int)max(min(temperatures[i], 255.0f), 0.0f) << " ";
         }
      }
   }
}

void printHelpPage(char *program)
{
   cout << "Simulates a simple heat diffusion." << endl;
   cout << endl << "Usage:" << endl;
   cout << "\t" << program << " INPUT_PATH OUTPUT_PATH" << endl << endl;
}

int main(int argc, char **argv)
{
   // Initialize MPI
   MPI_Init(&argc, &argv);
   int worldSize, myRank;
   MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
   MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

   if (argc == 1) {
      if (myRank == 0) {
         printHelpPage(argv[0]);
      }
      MPI_Finalize();
      exit(0);
   } else if (argc != 3) {
      if (myRank == 0) {
         printHelpPage(argv[0]);
      }
      MPI_Finalize();
      exit(1);
   }

   // Read the input instance.
   int width, height;  // Width and height of the matrix.
   vector<Spot> spots; // Spots with permanent temperature.
   if (myRank == 0) {
      tie(width, height, spots) = readInstance(argv[1]);
   }
   MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);
   MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);

   high_resolution_clock::time_point start = high_resolution_clock::now();

   // Fill this array on processor with rank 0. It must have height * width
   // elements and it contains the linearized matrix of temperatures in
   // row-major order (see
   // https://en.wikipedia.org/wiki/Row-_and_column-major_order)
   float *map = new float[width * height];
   int *mask = new int[width * height];
   bool *local_accuracy = new bool[worldSize];
   bool achieved_accuracy = false;
   if (myRank == 0) {
      for (int y = 0; y < height; ++y) {
         for (int x = 0; x < width; ++x) {
            // Handle borders
            // top
            if (y == 0) {
               if (x == 0 && y == 0)
                  GET_MASK(x, y) = LEFT_TOP;
               else if (x == width - 1 && y == 0)
                  GET_MASK(x, y) = RIGHT_TOP;
               else
                  GET_MASK(x, y) = TOP;
            } else if (y == height - 1) { // bottom
               if (x == width - 1 && y == height - 1)
                  GET_MASK(x, y) = RIGHT_BOTTOM;
               else if (x == 0 && y == height - 1)
                  GET_MASK(x, y) = LEFT_BOTTOM;
               else
                  GET_MASK(x, y) = BOTTOM;
            } else if (x == width - 1)
               GET_MASK(x, y) = RIGHT;
            else if (x == 0)
               GET_MASK(x, y) = LEFT;
            else
               GET_MASK(x, y) = 0;
         }
      }
      for (int i = 0; i < height * width; ++i)
         map[i] = DEFAULT_TEMPERATURE;
      for (auto &i : spots) {
         GET_MAP(i.mX, i.mY) = i.mTemperature;
         GET_MASK(i.mX, i.mY) = PERMANENT;
      }
      // Print mask
      // for (int y = 0; y < height; ++y) {
      //    for (int x = 0; x < width; ++x) {
      //       printf(" %2d", GET_MASK(x, y));
      //    }
      //    printf("\n");
      // }
   }

   MPI_Bcast(mask, width * height, MPI_INT, 0, MPI_COMM_WORLD);

   float *local_map = new float[width * height / worldSize];
   int *local_mask = new int[width * height / worldSize];
   do {
      MPI_Bcast(map, width * height, MPI_FLOAT, 0, MPI_COMM_WORLD);
      MPI_Scatter(map, 1, MPI_FLOAT, local_map, width * height / worldSize,
                  MPI_FLOAT, 0, MPI_COMM_WORLD);
      achieved_accuracy = true;
      for (int i = 0; i < width * height / worldSize; ++i) {
         if (mask[i + WORLD_CORRECTION] != PERMANENT) {
            switch (mask[i + WORLD_CORRECTION]) {
            case LEFT_TOP:
               local_map[i] =
                   (map[0] + map[1] + map[width] + map[width + 1]) / 4;
               break;
            case TOP:
               local_map[i] =
                   (map[i + WORLD_CORRECTION - 1] + map[i + WORLD_CORRECTION] +
                    map[i + WORLD_CORRECTION + 1] +
                    map[width - 1 + i + WORLD_CORRECTION] +
                    map[width + i + WORLD_CORRECTION] +
                    map[i + WORLD_CORRECTION + width + 1]) /
                   6;
               break;
            case RIGHT_TOP:
               local_map[i] =
                   (map[i + WORLD_CORRECTION - 1] + map[i + WORLD_CORRECTION] +
                    map[i + WORLD_CORRECTION + width - 1] +
                    map[i + WORLD_CORRECTION + width]) /
                   4;
               break;
            case RIGHT:
               local_map[i] =
                   (map[i + WORLD_CORRECTION - width - 1] +
                    map[i + WORLD_CORRECTION - width] +
                    map[i + WORLD_CORRECTION - 1] + map[i + WORLD_CORRECTION] +
                    map[i + WORLD_CORRECTION + width - 1] +
                    map[i + WORLD_CORRECTION + width]) /
                   6;
               break;
            case RIGHT_BOTTOM:
               local_map[i] =
                   (map[i + WORLD_CORRECTION - width - 1] +
                    map[i + WORLD_CORRECTION - width] +
                    map[i + WORLD_CORRECTION - 1] + map[i + WORLD_CORRECTION]) /
                   4;
               break;
            case BOTTOM:
               local_map[i] =
                   (map[i + WORLD_CORRECTION - width - 1] +
                    map[i + WORLD_CORRECTION - width] +
                    map[i + WORLD_CORRECTION - width + 1] +
                    map[i + WORLD_CORRECTION - 1] + map[i + WORLD_CORRECTION] +
                    map[i + WORLD_CORRECTION + 1]) /
                   6;
               break;
            case LEFT_BOTTOM:
               local_map[i] =
                   (map[i + WORLD_CORRECTION - width] +
                    map[i + WORLD_CORRECTION - width + 1] +
                    map[i + WORLD_CORRECTION] + map[i + WORLD_CORRECTION + 1]) /
                   4;
               break;
            case LEFT:
               local_map[i] =
                   (map[i + WORLD_CORRECTION - width] +
                    map[i + WORLD_CORRECTION - width + 1] +
                    map[i + WORLD_CORRECTION] + map[i + WORLD_CORRECTION + 1] +
                    map[i + WORLD_CORRECTION + width] +
                    map[i + WORLD_CORRECTION + width + 1]) /
                   6;
               break;

            default: // Normal
               local_map[i] =
                   (map[i + WORLD_CORRECTION - width - 1] +
                    map[i + WORLD_CORRECTION - width] +
                    map[i + WORLD_CORRECTION - width + 1] +
                    map[i + WORLD_CORRECTION + 1] +
                    map[i + WORLD_CORRECTION + width + 1] +
                    map[i + WORLD_CORRECTION + width] +
                    map[i + WORLD_CORRECTION + width - 1] +
                    map[i + WORLD_CORRECTION - 1] + map[i + WORLD_CORRECTION]) /
                   9;
               break;
            }

            if (achieved_accuracy &&
                abs(map[i + WORLD_CORRECTION] - local_map[i]) <= ACCURACY)
               achieved_accuracy = true;
            else
               achieved_accuracy = false;
         } else { // permanent spots
            local_map[i] = map[i + WORLD_CORRECTION];
         }
      }
      MPI_Gather(&achieved_accuracy, 1, MPI_CXX_BOOL, local_accuracy, worldSize,
                 MPI_CXX_BOOL, 0, MPI_COMM_WORLD);
      MPI_Gather(local_map, width * height / worldSize, MPI_FLOAT, map,
                 width * height / worldSize, MPI_FLOAT, 0, MPI_COMM_WORLD);
      if (myRank == 0) {
         achieved_accuracy = true;
         for (int i = 0; i < worldSize; ++i)
            if (!local_accuracy[i]) {
               achieved_accuracy = false;
               break;
            }
      }
      MPI_Bcast(&achieved_accuracy, 1, MPI_CXX_BOOL, 0, MPI_COMM_WORLD);

   } while (!achieved_accuracy);
   delete[] local_map, local_mask;

   // if (myRank == 0) {
   //    for (int y = 0; y < height; ++y) {
   //       for (int x = 0; x < width; ++x) {
   //          printf(" %3.0f", GET_MAP(x, y));
   //       }
   //       printf("\n");
   //    }
   // }

   //-----------------------\\

   double totalDuration =
       duration_cast<duration<double>>(high_resolution_clock::now() - start)
           .count();
   cout << "computational time: " << totalDuration << " s" << endl;

   if (myRank == 0) {
      string outputFileName(argv[2]);
      writeOutput(myRank, width, height, outputFileName, map);
   }

   delete[] map, mask, local_accuracy;

   MPI_Finalize();
   return 0;
}

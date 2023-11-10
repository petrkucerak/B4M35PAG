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
#define GET_MAP(x, y) map[(x) + width * (y)]
#define GET_MASK(x, y) mask[(x) + width * (y)]
#define GET_TMP_MAP(x, y) tmp_map[(x) + width * (y)]

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
                 const string outputFileName, const vector<float> &temperatures)
{
   // Draw the output image in Netpbm format.
   ofstream file(outputFileName);
   if (file.is_open()) {
      if (myRank == 0) {
         file << "P2\n" << width << "\n" << height << "\n" << 255 << "\n";
         for (auto temperature : temperatures) {
            file << (int)max(min(temperature, 255.0f), 0.0f) << " ";
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

   high_resolution_clock::time_point start = high_resolution_clock::now();

   // Fill this array on processor with rank 0. It must have height * width
   // elements and it contains the linearized matrix of temperatures in
   // row-major order (see
   // https://en.wikipedia.org/wiki/Row-_and_column-major_order)
   vector<float> map(width * height, DEFAULT_TEMPERATURE);
   vector<float> tmp_map(width * height);
   vector<int> mask(width * height, 0);
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
         }
      }
      for (auto &i : spots) {
         GET_MAP(i.mX, i.mY) = i.mTemperature;
         GET_MASK(i.mX, i.mY) = PERMANENT;
      }

      // for (int y = 0; y < height; ++y) {
      //    for (int x = 0; x < width; ++x) {
      //       printf(" %2d", GET_MASK(x, y));
      //    }
      //    printf("\n");
      // }

      do {
         achieved_accuracy = true;
         for (unsigned int i = 0; i < map.size(); ++i) {
            if (mask[i] != PERMANENT) {
               switch (mask[i]) {
               case LEFT_TOP:
                  tmp_map[i] =
                      (map[0] + map[1] + map[width] + map[width + 1]) / 4;
                  break;
               case TOP:
                  tmp_map[i] =
                      (map[i - 1] + map[i] + map[i + 1] + map[width - 1 + i] +
                       map[width + i] + map[i + width + 1]) /
                      6;
                  break;
               case RIGHT_TOP:
                  tmp_map[i] = (map[i - 1] + map[i] + map[i + width - 1] +
                                map[i + width]) /
                               4;
                  break;
               case RIGHT:
                  tmp_map[i] =
                      (map[i - width - 1] + map[i - width] + map[i - 1] +
                       map[i] + map[i + width - 1] + map[i + width]) /
                      6;
                  break;
               case RIGHT_BOTTOM:
                  tmp_map[i] = (map[i - width - 1] + map[i - width] +
                                map[i - 1] + map[i]) /
                               4;
                  break;
               case BOTTOM:
                  tmp_map[i] =
                      (map[i - width - 1] + map[i - width] +
                       map[i - width + 1] + map[i - 1] + map[i] + map[i + 1]) /
                      6;
                  break;
               case LEFT_BOTTOM:
                  tmp_map[i] = (map[i - width] + map[i - width + 1] + map[i] +
                                map[i + 1]) /
                               4;
                  break;
               case LEFT:
                  tmp_map[i] =
                      (map[i - width] + map[i - width + 1] + map[i] +
                       map[i + 1] + map[i + width] + map[i + width + 1]) /
                      6;
                  break;

               default: // Normal
                  tmp_map[i] = (map[i - width - 1] + map[i - width] +
                                map[i - width + 1] + map[i + 1] +
                                map[i + width + 1] + map[i + width] +
                                map[i + width - 1] + map[i - 1] + map[i]) /
                               9;
                  break;
               }

               if (achieved_accuracy && abs(map[i] - tmp_map[i]) <= ACCURACY)
                  achieved_accuracy = true;
               else
                  achieved_accuracy = false;
            } else { // permanent spots
               tmp_map[i] = map[i];
            }
         }

         // copy memmory
         map.assign(tmp_map.begin(), tmp_map.end());

         // printf("\n\n\n");
         // for (int y = 0; y < height; ++y) {
         //    for (int x = 0; x < width; ++x) {
         //       printf(" %3.0f", GET_MAP(x, y));
         //    }
         //    printf("\n");
         // }

      } while (!achieved_accuracy);

      for (int y = 0; y < height; ++y) {
         for (int x = 0; x < width; ++x) {
            printf(" %3.0f", GET_MAP(x, y));
         }
         printf("\n");
      }
   }

   //-----------------------\\

   double totalDuration =
       duration_cast<duration<double>>(high_resolution_clock::now() - start)
           .count();
   cout << "computational time: " << totalDuration << " s" << endl;

   if (myRank == 0) {
      string outputFileName(argv[2]);
      writeOutput(myRank, width, height, outputFileName, map);
   }

   MPI_Finalize();
   return 0;
}

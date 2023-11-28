#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <list>
#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <stdio.h>
#include <tuple>
#include <vector>

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define ORANGE "\033[38;5;208m"
#define PURPLE "\033[38;5;129m"
#define BROWN "\033[38;5;94m"
#define PINK "\033[38;5;206m"
#define DARK_GREEN "\033[38;5;22m"

using namespace std;

const char *colors[] = {RED,    GREEN,  BLUE,  YELLOW, MAGENTA,   CYAN,
                        ORANGE, PURPLE, BROWN, PINK,   DARK_GREEN};

#define ccout(my_rank) cout << colors[my_rank]
#define cendl RESET << endl

template <typename T>
static inline string vector_to_string(const vector<T> &vec)
{
   std::stringstream result;
   std::copy(vec.begin(), vec.end(), std::ostream_iterator<T>(result, ", "));
   string str_vec = result.str();
   str_vec.pop_back(); // remove ' '
   str_vec.pop_back(); // remove ','
   string output_string = "<" + str_vec + ">";
   return output_string;
}

template <typename T> static inline string stack_to_string(const stack<T> &s)
{
   std::stringstream result;
   result << "{";
   std::stack<T> tempStack = s;
   int k = 1;
   while (!tempStack.empty()) {
      // result << (k++)<<". : ";
      result << tempStack.top();
      tempStack.pop();
      if (!tempStack.empty()) {
         result << ", ";
      }
   }
   result << "}";
   return result.str();
}

template <typename T> static inline string queue_to_string(const queue<T> &q)
{
   std::stringstream result;
   std::queue<T> tempQueue = q; // Creation copy of queue
   int k = 1;
   result << "{";
   while (!tempQueue.empty()) {
      // result << (k++)<<". : ";
      result << tempQueue.front();
      tempQueue.pop();
      if (!tempQueue.empty()) {
         result << ", ";
      }
   }
   result << "}";
   return result.str();
}

template <typename T> static inline string set_to_string(const set<T> &q)
{
   std::stringstream result;
   result << "{";
   int k = 1;
   for (auto it = std::begin(q); it != std::end(q); ++it) {
      // result << (k++)<<". : "<<*it;
      result << *it;
      if (std::next(it) != std::end(q)) {
         result << ", ";
      }
   }

   result << "}";
   return result.str();
}

template <typename T> static inline string list_to_string(const list<T> &l)
{
   std::stringstream result;
   std::copy(l.begin(), l.end(), std::ostream_iterator<T>(result, ", "));
   string str_vec = result.str();
   str_vec.pop_back(); // remove ' '
   str_vec.pop_back(); // remove ','
   string output_string = "<" + str_vec + ">";
   return output_string;
}

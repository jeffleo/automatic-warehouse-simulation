#include <iostream>

#include <vector>
#include <queue>
#include <stack>
#include <deque>
#include <utility> // for std::pair
#include <map>

#include <string>
#include <cpen333/util.h>

int main() {

  // Rather than deal with dynaimic arrays and memory management, it's often much easier (and safer) to use
  // containers found in the "standard template library" (STL)

  // These make use of "templates", meaning they can be adapted to use any type that you specify

  //=======================================================
  // VECTOR
  //=======================================================
  // A vector is like a dynamic array, and can be accessed like one.  You can add to and remove from the back of it.
  // It also has methods to check its size, if it is empty, etc...
  std::vector<int> vec;
  for (int i=0; i<10; ++i) {
    vec.push_back(i+1);  // append to back
  }

  vec.pop_back();        // remove the last element

  std::cout << "Contents of vec: {";
  for (size_t i=0; i<vec.size(); ++i) {
    std::cout << " " << vec[i];
  }
  std::cout << " }" << std::endl << std::endl;


  //=======================================================
  // QUEUE
  //=======================================================
  // A queue adds to the back and removes from the front (like lining up at the grocery store).
  // You cannot access individual elements of the queue except for the front/back.
  // i.e. first-in, first-out (FIFO)
  std::queue<double> queue;
  // add some doubles to the queue
  queue.push(1.0);
  queue.push(2.0);
  queue.push(3.0);
  queue.push(4.0);

  std::cout << "Queue size: " << queue.size() << " , Front of queue: " << queue.front() << std::endl;
  queue.pop();  // remove an item
  std::cout << "Queue size: " << queue.size() << " , Front of queue: " << queue.front() << std::endl << std::endl;

  //=======================================================
  // STACK
  //=======================================================
  // A stack adds to the back and removes from the back (like stacking books)
  // You cannot access elements of the stack except for the "top"
  std::stack<short> stack;
  stack.push(5);
  stack.push(6);
  stack.push(7);
  stack.push(8);
  std::cout << "Stack size: " << stack.size() << " , Top of stack: " << stack.top() << std::endl;
  stack.pop();  // remove an item
  std::cout << "Stack size: " << stack.size() << " , Top of stack: " << stack.top() << std::endl << std::endl;

  //=======================================================
  // DEQUE
  //=======================================================
  // A deque is like a queue and stack combined.  You can add to or remove from either end.
  // You can also access individual elements like in a vector or an array.
  std::deque<std::string> deq;
  deq.push_front("hello");
  deq.push_back("world");
  deq.push_front("says");
  deq.push_front("Dijkstra");
  deq.push_back("!");

  std::cout << "Contents of deq: ";
  for (size_t i=0; i<deq.size(); ++i) {
    std::cout << " " << deq[i];
  }
  std::cout << std::endl << std::endl;

  //=======================================================
  // PAIR
  //=======================================================
  // A pair stores two values, which can be of different types
  std::pair<int, std::string> pair(10, "happy");
  std::cout << "Contents of pair: {" << pair.first << ", " << pair.second <<  "}" << std::endl << std::endl;

  //=======================================================
  // MAP
  //=======================================================
  // A map associates a "key" with a "value".
  // This is EXTREMELY useful, particularly for associating data with other objects.
  std::map<std::string,int> agemap;
  agemap.insert(std::pair<std::string,int>("Jack", 18));  // can insert as pairs
  agemap.insert(std::pair<std::string,int>("Jill", 20));

  // can also insert through element access, will create new element if key doesn't exist
  agemap["Adam"] = 21;
  agemap["Amy"] = 21;
  agemap["Patrick"] = 22;
  agemap.erase("Adam");         // remove Adam from the group
  agemap["Amy"] = 22;           // change Amy's age

  // show contents of map
  std::cout << "Contents of agemap: {";

  // Here I use a ranging "for-each" loop to go through all the key->value pairs.
  bool first = true;
  for (const std::pair<std::string,int>& p : agemap) {
    if (!first) {
      std::cout << ","; // add comma if not first element
    }
    std::cout << " " <<  p.first << "=" << p.second;
    first = false;
  }
  std::cout << " }" << std::endl;

  cpen333::pause();
  return 0;
}
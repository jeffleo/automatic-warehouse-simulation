#include <iostream>
#include <string>
#include <thread>    // for spawning threads
#include <cpen333/util.h>

//
//  Interprocess Shared Memory (a.k.a Shared Memory, Datapools, or Mapped Memory) are simply blocks of shared memory
//  accessible by multiple threads/processes.  Since threads within a single process all share memory anyways, we can
//  just use either a shared object, or a global variable
//
//  There is a subtlety: the shared data block should be contiguous and completely self-contained.  This means we
//  can't store regular object pointers within the memory block, since those will inevitably refer to memory outside of
//  the block itself.  While this may not be an issue for threads which would also share that outside memory and address
//  space, it becomes a really big issue for processes, which don't.  From the perspective of a different process,
//  that pointer-based memory address may be invalid, or refer to some other completely unrelated block of memory.
//
//  Even more subtlety: we also need to avoid containers, and other classes which internally may store data on the
//  heap.  This includes strings, vectors, maps, etc...
//

// Layout of block of shared memory stored within a struct
// Since all members are fixed-length basic types, this is a self-contained contiguous block of memory.  In C++, the
// memory layout is gauranteed, with members arranged in order [ 20 chars, 1 long, 1 double ].  The actual number of
// bytes may change from system to system, but the addresses can be computed using the sizeof() operator
struct SharedData {
  char movie[20];    // title
  long length;       // in seconds
  double rating;     // on a 5* scale
};

// create a function to run in a thread that takes a reference to the shared data and prints its info
void ThreadFunction(const std::string &name, SharedData& data) {

  // print out the data in the shared memory that was stored there by the parent
  std::cout << "Child " << name << " read value for Movie = " << data.movie << std::endl;
  std::cout << "Child " << name << " read value for length = " << data.length << std::endl;
  std::cout << "Child " << name << " read values for rating = " << data.rating << std::endl << std::endl;

  std::cout << "Press Enter to quit child " << name << std::endl;
  std::cin.get();  // pause so you can view output before child dies

}

int main() {
  
  SharedData data;
  
  std::cout << "Parent Writing value 'Toy Story 2' to movie variable....." << std::endl;
  // copy data from a string to the output array, leave room for a terminating 0
  std::string("Toy Story 2").copy(data.movie, 19, 0);
  data.movie[19] = 0;

  std::cout << "Parent Writing value '95' to length variable....." << std::endl;
  data.length = 95;

  std::cout << "Parent Writing value '4.9' to ratings variable....." << std::endl;
  data.rating = 4.9;

  // Now that we have created the shared memory object and have stored data in it, it is safe to create
  // a child thread that can access the data
  std::cout << "Press Enter to launch thread" << std::endl;
  std::cin.get();


  // Spawn child thread, pass it the shared data by reference
  // NOTE:  thread functions require you to explicitly mark variables passed by reference using std::ref()
  // NOTE2: ANYTHING PASSED BY REFERENCE MUST REMAIN IN SCOPE UNTIL THE THREADS COMPLETE
  //        Otherwise, when the thread tries to access the data, it may have been deleted off the stack already
  std::thread t1(&ThreadFunction, "Thread1", std::ref(data));
  t1.join();  // wait for thread to complete

  // ADVANCED:
  // You can also use a lambda expression to handle the passing by reference.
  // This lambda "captures" data by-reference, then calls the ThreadFunction on it
  auto lambda = [&data](const std::string& name) {
    ThreadFunction(name, data);
  };
  // lambda has only one argument now
  std::thread t2(lambda, "Thread2");
  t2.join();

  cpen333::pause();

}
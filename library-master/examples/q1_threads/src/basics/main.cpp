#include <iostream> // input/output
#include <thread>   // standard template library (STL) threads package
#include <chrono>   // STL chrono (time) package
#include <cpen333/util.h>

//
//  Threads represent things that can run independently, and in parallel, but can share resources within a program
//  In C++, threads can be passed anything that can by called using round brackets: (...)
//  When the thread starts, it will run in parallel with the rest of your code.  To wait for a spawned thread to finish,
//  you can "join" it to the current thread.
//

// The most basics kind of thing a thread can be passed is a function
void thread_fn1(int n) {
  for(int i = 0; i < n; ++i) {
    std::cout << "Hello from child thread 1....." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // sleep for 50 ms
  }
}

// A more powerful thing to pass a thread is a struct or class.  Make sure to overload the () operator
// so an instance can be called using my_instance();
struct thread_fn2 {
  // () operator with one argument: int
  void operator()(int n) const {
    for(int i = 0; i < n; ++i) {
      std::cout << "Hello from child thread 2....." << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(50)); // sleep for 50 ms
    }
  }
};

// For short, simple functions, it's often easier to write a lambda expression (advanced).  These are typically
// written within method bodies, next to the other variables
auto thread_fn3 = [](int n, int data) {
  for(int i = 0; i < n; ++i) {
    std::cout << "Hello from child thread 3...I Have been given the data " << data << std::endl;;
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // sleep for 50 ms
  }
};

// this is the processes main or parent thread
int main() {
  std::cout << "Parent Thread: Creating Child Threads" << std::endl;

  // threads are defined using
  //    std::thread( function, arguments... )
  // which will start a thread that automically runs function(arguments...)


  std::thread t1(&thread_fn1, 50);      // function pointer.  This will also work without the &,
                                        // since template magic will deduce what you mean.
  thread_fn2 thread_fn2_inst;           // create an instance of our thread_fn2 struct
  std::thread t2(thread_fn2_inst, 30);  // pass in instance
  std::thread t3(thread_fn3, 20, 52);   // lambda (pass in directly)

  for(int i = 0; i < 40; i ++) {
    std::cout << "..........Hello from Parent thread ....." << std::endl ;
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // sleep for 200 ms
  }

  // wait for threads to die
  t1.join();
  t2.join();
  t3.join();

  cpen333::pause();

  return 0 ;			// end the processes main thread
}
#include <iostream>
#include <chrono>
#include <thread>

#include <cpen333/util.h>

int main(int argc, char* argv[]) {

  std::cout << "argc = " << argc << std::endl;
  for(int i = 0; i < argc; ++i ) {
    std::cout << "arg[" << i << "] = " << argv[i] << std::endl;
  }

  // pause for user input
  cpen333::pause();

  for( int i = 0; i < 300; ++i) {
    std::cout << "Hello from child process 3...." << std::endl;
    // sleep 50 ms
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  return 0;

}
#include <iostream>
#include <chrono>   // for times
#include <thread>   // for sleep

int main() {

  for(int i = 0; i < 250; ++i) {
    std::cout << "Hello from child process 2...." << std::endl;
    // sleep 50 ms
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  return 0;
}
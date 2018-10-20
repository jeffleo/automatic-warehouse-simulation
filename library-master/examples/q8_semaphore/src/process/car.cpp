#include <iostream>
#include <cstdlib>   // atoi
#include <thread>
#include <cpen333/process/semaphore.h>

int main(int argc, char* argv[]) {

  // extract id from argv
  int id = 0;
  if (argc > 1) {
    id = atoi(argv[1]);
  }

  // connect to semaphore, make sure same name (best to have a common header with name definition)
  cpen333::process::semaphore  pumps("gas_station_pumps", 3);

  std::cout << "Car " << id << " waiting for gas pump to be free" << std::endl;
  // wait for pump to be free
  pumps.wait();
  std::cout << "Car " << id << " at pump, getting gas..." << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << "Car " << id << " leaving pump" << std::endl;
  // let others know the pump is now free
  pumps.notify();

  return 0;
}
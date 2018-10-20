#include <iostream>
#include <vector>
#include <thread>

#include <cpen333/util.h>
#include <cpen333/thread/semaphore.h>

//
//  Semaphores are counting objects that represent a certain number of "resources", and allow threads to wait until
//  one of the resources is available.  To acquire a resource, you need to wait() on it, which will block until
//  the semaphore's count > 0.  Once the thread passes through, it will safely decrement the count.
//  To release the resource for others to use, you notify() the semaphore, which will increase its internal count.
//
//  Some semaphores allow you to specify a maximum count, so that calling notify() many times will only increase
//  the internal count up to the maximum value.  The ones used here do not have this property, though you can easily
//  create your own using a shared "count" variable.
//
//

void car(cpen333::thread::semaphore& pumps, int id) {
  std::cout << "Car " << id << " waiting for gas pump to be free" << std::endl;
  // wait for pump to be free
  pumps.wait();
  std::cout << "Car " << id << " at pump, getting gas..." << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  std::cout << "Car " << id << " leaving pump" << std::endl;
  // let others know the pump is now free
  pumps.notify();
}

int main() {

  // create a gas station with 3 pumps
  cpen333::thread::semaphore pumps(3);

  // create 100 cars to go through pumps
  std::vector<std::thread> cars;
  for (int i=0; i<100; ++i) {
      cars.push_back(std::thread(&car, std::ref(pumps), i+1));
  }

  // wait for all threads to terminate
  for (std::thread& car : cars) {
    car.join();
  }

  cpen333::pause();

  return 0;
}
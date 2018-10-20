#include <iostream>
#include <vector>
#include <cpen333/util.h>
#include <cpen333/process/subprocess.h>
#include <cpen333/process/semaphore.h>
#include <cpen333/process/unlinker.h>

//
//  Semaphores are counting objects that represent a certain number of "resources", and allow processes to wait until
//  one of the resources is available.  To acquire a resource, you need to wait() on it, which will block until
//  the semaphore's count > 0.  Once the thread passes through, it will safely decrement the count.
//  To release the resource for others to use, you notify() the semaphore, which will increase its internal count.
//
//  Some semaphores allow you to specify a maximum count, so that calling notify() many times will only increase
//  the internal count up to the maximum value.  The ones used here do not have this property, though you can easily
//  create your own using a shared "count" variable.
//

int main() {

  // create a gas station with 3 pumps
  cpen333::process::semaphore  pumps("gas_station_pumps", 3);

  // Having an RAII unlinker here is especially important since we will be launching a whole bunch of processes,
  // which might get very messy.  We want to make sure our named semaphore is released properly, otherwise the next
  // time we try to run this program the semaphore may still exist with a count of 0, blocking everyone!!!
  cpen333::process::unlinker<decltype(pumps)> unlinker(pumps);

  std::vector<cpen333::process::subprocess> cars;

  // spawn 30 processes :S
  for (int i=0; i<30; ++i) {
    std::vector<std::string> args;
    args.push_back("./car");
    args.push_back(std::to_string(i+1));
    cars.push_back(cpen333::process::subprocess(args, true, false));
  }

  // wait for all processes to finish
  for (auto& car : cars) {
    car.join();
  }

  cpen333::pause();

  return 0;
}
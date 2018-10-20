#include <iostream>
#include <cstdlib>   // atoi
#include <thread>
#include <cpen333/process/condition_variable.h>
#include <cpen333/process/mutex.h>
#include <cpen333/process/shared_memory.h>
#include "restaurant.h"

int main(int argc, char* argv[]) {

  // get id from arguments (if exists)
  int id = 0;
  if (argc > 1) {
    id = atoi(argv[1]);
  }

  // grab cv/mutex/shared memory
  cpen333::process::condition_variable cv(CONDITION_VARIABLE_RESTAURANT_CV_NAME);
  cpen333::process::mutex mutex(CONDITION_VARIABLE_RESTAURANT_MUTEX_NAME);
  cpen333::process::shared_object<DishData> dishes(CONDITION_VARIABLE_RESTAURANT_DATA_NAME);
  
  std::cout << "Customer " << id << " arrived" << std::endl;

  // will eat 50 meals
  // needs to wait until there's a clean dish
  for (int i=0; i<50; ++i) {
    std::unique_lock<decltype(mutex)> lock(mutex);
    cv.wait(lock, [&dishes](){ return dishes->clean > 0; });

    // I now have exclusive access, I can take a clean dish
    --dishes->clean;
    lock.unlock();   // let others use the resources (i.e. access the data if they need to)

    // eat on the dish
    // std::cout << "Customer " << id << " eating" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // add to dirty stack
    lock.lock();
    ++dishes->dirty;
    lock.unlock();
    // let all dishwashers know about dirty dish
    cv.notify_all();
  }

  std::cout << "Customer " << id << " leaving" << std::endl;
}
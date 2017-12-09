#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

#include <cpen333/util.h>

//
// Condition variables allow you to wait for a certain condition, as defined by predicate, to hold true.  It is more
// general than a basic condition.  The predicate evaluation needs to be done under a lock (since it will inevitably
// depend on some sort of outside data).  The wait function is defined as:
//       condition_variable.wait(lock, predicate)
// where predicate is some boolean functional that can be evaluated using the () operator.  Internally, the condition
// variable will unlock the lock and wait to be notified.  Once notified, it will acquire the lock again, check the
// predicate, and if predicate() is true, will exit the wait function and continue execution.  The cv is notified using
//       condition_variable.notify_one(),  or condition_variable.notify_all()
// to either wake up one thread waiting on the condition, or all threads.  The notification itself does not need to be
// done under a lock.  Typical program flow is:
//
//    Waiting:     acquire lock, call cv.wait(lock, predicate)
//    Notifying:   acquire lock, modify data that predicate depends on, release lock, notify
//

struct DishData {
  int dirty;
  int clean;
  int washing;
  bool quit;
};

void dishwasher(int id, std::condition_variable& cv, std::mutex& mutex, DishData& dishes) {

  // protect data
  std::unique_lock<std::mutex> lock(mutex);
  std::cout << "Dishwasher " << id << " arrived" << std::endl;

  // loop while it's not quittin' time
  while (!dishes.quit) {

    std::cout << "Dishwasher " << id << " taking a break" << std::endl;

    // wait until there are at least 20 dirty dishes (or it's quitting time, otherwise we may get stuck here)
    cv.wait(lock, [&dishes]() { return dishes.dirty > 50 || dishes.quit; });

    std::cout << "Dishwasher " << id << " starting to wash dishes" << std::endl;

    // we now re-acquire the lock, let's do some dishes, but let others do some as well
    // this will require some lock juggling, needing to acquire it before accessing the count
    // and releasing while doing the dish so others can do some too

    while (dishes.dirty > 0) {
      --dishes.dirty;    // grab one first, without collisions
      ++dishes.washing;
      lock.unlock();

      // wash the dish
      std::this_thread::sleep_for(std::chrono::milliseconds(10));  // man, that's fast

      // add to stack of clean dishes
      lock.lock();
      --dishes.washing;
      ++dishes.clean;
      lock.unlock();

      // notify people that there's one new clean dish
      cv.notify_all();

      lock.lock();  // need to re-lock to check condition in while loop
    }
  }

  std::cout << "Dishwasher " << id << " leaving" << std::endl;
}

void customer(int id, std::condition_variable& cv, std::mutex& mutex, DishData& dishes) {

  std::cout << "Customer " << id << " arrived" << std::endl;

  // will eat 50 meals
  // needs to wait until there's a clean dish
  for (int i=0; i<50; ++i) {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [&dishes](){ return dishes.clean > 0; });

    // I now have exclusive access, I can take a clean dish
    --dishes.clean;
    lock.unlock();   // let others use the resources (i.e. access the data if they need to)

    // eat on the dish
    // std::cout << "Customer " << id << " eating" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // add to dirty stack
    lock.lock();
    ++dishes.dirty;
    lock.unlock();
    // let all dishwashers know about dirty dish
    cv.notify_all();
  }

  std::cout << "Customer " << id << " leaving" << std::endl;
}

int main() {

  std::condition_variable cv;
  std::mutex mutex;
  // 200 clean dishes available
  DishData dishes = {0, 200, 0, false};

  std::vector<std::thread> washers;
  std::vector<std::thread> customers;

  // 2 washers, 20 customers
  for (int i=0; i<2; ++i) {
    washers.push_back(std::thread(&dishwasher, i+1, std::ref(cv), std::ref(mutex), std::ref(dishes)));
  }
  for (int i=0; i<20; ++i) {
    customers.push_back(std::thread(&customer, i+1, std::ref(cv), std::ref(mutex), std::ref(dishes)));
  }

  // wait for some time
  std::this_thread::sleep_for(std::chrono::seconds(30));

  // terminate
  {
    std::lock_guard<std::mutex> lock(mutex);
    dishes.quit = true;
  }
  cv.notify_all();  // let everyone know about termination (wake up both dishwashers)

  // wait for all threads to finish
  for (auto& dw : washers) {
    dw.join();
  }
  for (auto& c : customers) {
    c.join();
  }

  cpen333::pause();

  return 0;
}
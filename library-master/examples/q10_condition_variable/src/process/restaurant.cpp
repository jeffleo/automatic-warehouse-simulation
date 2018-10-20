#include <iostream>
#include <thread>

#include <cpen333/util.h>
#include <cpen333/process/subprocess.h>
#include <cpen333/process/condition_variable.h>
#include <cpen333/process/mutex.h>
#include <cpen333/process/shared_memory.h>
#include <cpen333/process/unlinker.h>
#include "restaurant.h"


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

int main() {

  cpen333::process::condition_variable cv(CONDITION_VARIABLE_RESTAURANT_CV_NAME);
  cpen333::process::mutex mutex(CONDITION_VARIABLE_RESTAURANT_MUTEX_NAME);
  cpen333::process::shared_object<DishData> dishes(CONDITION_VARIABLE_RESTAURANT_DATA_NAME);
  // unlinkers
  cpen333::process::unlinker<decltype(cv)> cvu(cv);
  cpen333::process::unlinker<decltype(mutex)> mutexu(mutex);
  cpen333::process::unlinker<decltype(dishes)> dishesu(dishes);

  // 200 clean dishes available
  dishes->quit = false;
  dishes->dirty = 0;
  dishes->clean = 200;
  dishes->washing = 0;

  std::vector<cpen333::process::subprocess> washers;
  std::vector<cpen333::process::subprocess> customers;

  // 2 washers, 20 customers
  for (int i=0; i<2; ++i) {
    // dishwashers in their own separate window
    std::vector<std::string> args;
    args.push_back("./dishwasher");
    args.push_back(std::to_string(i+1));
    washers.push_back(cpen333::process::subprocess(args, true, true));
  }

  for (int i=0; i<20; ++i) {
    // customers are within same window
    std::vector<std::string> args;
    args.push_back("./customer");
    args.push_back(std::to_string(i+1)); 
    customers.push_back(cpen333::process::subprocess(args, true, false));
  }

  // wait for some time
  std::this_thread::sleep_for(std::chrono::seconds(30));

  // terminate
  {
    std::lock_guard<decltype(mutex)> lock(mutex);
    dishes->quit = true;
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
#include <cstdlib>

#include "common.h"
#include <cpen333/process/shared_memory.h>
#include <cpen333/process/mutex.h>

int main(int argc, char* argv[]) {

  // parse counter from argv
  int n = 100;
  if (argc > 1) {
    n = atoi(argv[1]);
  }

  // shared memory
  cpen333::process::shared_object<SharedData> data(MUTEX_SHARED_MEMORY_NAME);
  cpen333::process::mutex mutex(MUTEX_MUTEX_NAME);

  // safely increment shared data a
  for (int i=0; i<n; ++i) {
    // lock the mutex, increment, then unlock
    // NOTE: the shared data access MAY actually be a concern for throwing an exception
    //       It might be best to use a lock_guard here, though I'll leave the mutex for demonstration
    mutex.lock();
    data->a = data->a+1;
    mutex.unlock();
  }

  return 0;
}
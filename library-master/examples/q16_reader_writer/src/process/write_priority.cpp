#include <iostream>
#include <thread>
#include <vector>
#include <cpen333/util.h>
#include <cpen333/process/shared_mutex.h>
#include <cpen333/process/unlinker.h>

// NOTE: Here we are actually spawning 100s of threads instead of processes in order to save on system resources
//       We are still using the IPC mutexes, however, to simulate multiple processes

//
//  Data that is unchanging does not need to be protected using a mutex.  But what if data only changes "occasionally"?
//  That is, what if you end up reading from a set of data a lot more often that writing to it?  Multiple readers
//  who promise not to try to modify the data should be able to read the data at the same time.
//
//  This is the purpose of "shared" mutexes.  When locked in "shared" mode, anyone who promises not to modify the
//  data can be granted access at the same time.  When someone wants to write to the data, they need to request
//  access and lock the mutex in "exclusive" mode, allowing only one thread to access the data.
//
//  The details of shared mutexes can be tricky, but in general there are three priority modes:
//      read_priority: if readers are currently reading in shared mode, any new reader will be granted access,
//                     writers will then be locked out until there are no more readers.  This may lead to writer
//                     starvation.
//      write_priority: once exclusive access is requested, all further readers are blocked out until all writers
//                      have finished their jobs.  This might be important if there will be very many readers, and only
//                      a few writers.  However, if there are many writers, then this can starve out the readers.
//      fair_priority:  readers and writers will share priority, usually based on some kind of alternation scheme.
//

// template function is compiled for every type of MutexType
template<typename MutexType>
void read_thread(const std::string& mutex_name, size_t n) {
  MutexType mutex(mutex_name);                // load named mutex
  for (size_t i=0; i<n; ++i) {
    {
      // lock in "shared" mode
      std::shared_lock<MutexType> lock(mutex);
      std::cout << "R";
      std::cout.flush();  // flush read
      std::this_thread::sleep_for(std::chrono::milliseconds(10));  // reads are usually pretty fast
    }
    std::this_thread::yield();  // prevent any single thread from hogging
  }
}

// template function is compiled for every type of MutexType
template<typename MutexType>
void write_thread(const std::string& mutex_name, size_t n) {
  MutexType mutex(mutex_name);              // load named mutex
  for (size_t i=0; i<n; ++i) {
    {
      // lock in "exclusive" mode
      std::lock_guard<MutexType> lock(mutex);
      std::cout << "W";
      std::cout.flush();  // flush read
      std::this_thread::sleep_for(std::chrono::milliseconds(50));  // writes usually take longer
    }
    std::this_thread::yield();  // prevent any single thread from hogging
  }
}

int main() {

  std::vector<std::thread> readers;  // reader threads
  std::vector<std::thread> writers;  // writer threads

  // share a read-priority mutex
  std::cout << "Battle of the Readers vs Writers, write priority" << std::endl;

  // start separate threads with readers and writers
  cpen333::process::shared_mutex_exclusive wmutex("write_priority");  // gives preference to exclusive (in our case, writers)
  cpen333::process::unlinker<decltype(wmutex)> unlinker(wmutex);  // unlink when done

  for (int i=0; i<10; ++i) {
    writers.push_back(std::thread(&write_thread<decltype(wmutex)>, "write_priority", 50));
    readers.push_back(std::thread(&read_thread<decltype(wmutex)>, "write_priority", 50));
  }
  // wait until all threads complete, then dispose
  for (size_t i=0; i<readers.size(); ++i) {
    readers[i].join();
  }
  for (size_t i=0; i<writers.size(); ++i) {
    writers[i].join();
  }

  std::cout << std::endl << "Done." << std::endl;
  cpen333::pause();

  return 0;
}
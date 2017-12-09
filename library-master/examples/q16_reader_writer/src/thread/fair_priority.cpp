#include <iostream>
#include <thread>
#include <vector>
#include <cpen333/util.h>
#include <cpen333/thread/shared_mutex.h>

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
void read_thread(MutexType& mutex, size_t n) {
  for (size_t i=0; i<n; ++i) {
    {
      // lock in "shared" mode
      std::shared_lock<MutexType> lock(mutex);
      std::cout << "R";
      std::cout.flush();  // flush read
      // std::this_thread::sleep_for(std::chrono::milliseconds(100));  // reads are usually pretty fast
    }
    std::this_thread::yield();  // prevent this thread from hogging resources
  }
}

// template function is compiled for every type of MutexType
template<typename MutexType>
void write_thread(MutexType& mutex, size_t n) {
  for (size_t i=0; i<n; ++i) {
    {
      // lock in "exclusive" mode
      std::lock_guard<MutexType> lock(mutex);
      std::cout << "W";
      std::cout.flush();  // flush read
      // std::this_thread::sleep_for(std::chrono::milliseconds(100));  // writes usually take longer
    }
    std::this_thread::yield();  // prevent this thread from hogging resources
  }
}

int main() {

  std::vector<std::thread> readers;  // reader threads
  std::vector<std::thread> writers;  // writer threads

  // share a read-priority mutex
  std::cout << "Battle of the Readers vs Writers, fair priority" << std::endl;

  // start separate threads with readers and writers
  cpen333::thread::shared_mutex_fair fmutex;  // tries to give fair preference
  for (int i=0; i<100; ++i) {
    writers.push_back(std::thread(&write_thread<decltype(fmutex)>, std::ref(fmutex), 200));
    readers.push_back(std::thread(&read_thread<decltype(fmutex)>, std::ref(fmutex), 200));
  }
  // wait until all threads complete, then dispose
  for (int i=0; i<100; ++i) {
    readers[i].join();
    writers[i].join();
  }
  readers.clear();
  writers.clear();


  std::cout << std::endl << "Done." << std::endl;
  cpen333::pause();
  return 0;
}
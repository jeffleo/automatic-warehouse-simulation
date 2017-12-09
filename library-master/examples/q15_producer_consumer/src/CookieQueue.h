#ifndef COOKIEQUEUE_H
#define COOKIEQUEUE_H

#include <string>

#include <cpen333/process/semaphore.h>
#include <cpen333/process/shared_memory.h>
#include <cpen333/process/mutex.h>

// common name for shared access
#define COOKIE_QUEUE_NAME "_i_love_cookies_"

// Types of cookies
// the POISON flavour may sound bleak, but refers to the
// term Poison Pill, which signals consumers to stop
enum CookieType {
  CHOCOLATE_CHIP, OATMEAL_RAISIN, GINGER_SNAP, POISON
};

// Data type for our queue
struct Cookie {
  CookieType type;
  int baker_id;
  int cookie_id;

  Cookie(CookieType type, int baker_id, int cookie_id) :
    type(type), baker_id(baker_id), cookie_id(cookie_id) {}
};

// Circular buffer with semaphores to control access
class CookieQueue {
 protected:
  struct QueueInfo {
    size_t pidx;  // next index to put information at
    size_t cidx;  // next index to consume information from
    size_t size;  // size of circular buffer, in # of elements
    int initialized;    // magic number for detecting if memory is initialized
  };

  cpen333::process::shared_memory memory_;   // actual memory
  QueueInfo* info_;                          // pointer to queue information, will be at start of memory_
  Cookie* data_;                             // pointer to data in queue (in this case, cookies), after info_ in memory
  cpen333::process::mutex pmutex_;           // mutex for protecting memory modified by producers
  cpen333::process::mutex cmutex_;           // mutex for protecting memory modified by consumers
  cpen333::process::semaphore psem_;         // semaphore controlling when producer can add a cookie
  cpen333::process::semaphore csem_;         //     "            "         consumer can remove a cookie

 public:

  CookieQueue(const std::string& name, size_t size) :
      memory_(name + std::string("_memory_"), sizeof(QueueInfo)+size*sizeof(Cookie)), // reserve memory
      info_(nullptr), data_(nullptr), // will initialize these after memory valid
      pmutex_(name + std::string("_pmutex_")),
      cmutex_(name + std::string("_cmutex_")),
      psem_(name + std::string("_psem_"), size),  // start at size of queue
      csem_(name + std::string("_csem_"), 0) {    // start at zero

    // info is at start of memory block, followed by the actual data in the queue
    info_ = (QueueInfo*)memory_.get();
    data_ = (Cookie*)memory_.get(sizeof(QueueInfo));  // start of queue data is after Queue info

    // protect memory with a mutex (either one) to check if data is initialized, and initialize if not
    // This is only to stop multiple constructors from simultaneously trying to initialize data
    std::lock_guard<cpen333::process::mutex> lock(pmutex_);
    if (info_->initialized != 0x12345678) {
      info_->pidx = 0;
      info_->cidx = 0;
      info_->size = size;
      info_->initialized = 0x12345678;  // mark initialized
    }
  }

  // add cookie to queue
  void push(const Cookie &cookie) {

    // wait until room to push a cookie
    psem_.wait();

    size_t loc = 0;
    {
      // look at index, protect memory from multiple simultaneous pushes
      std::lock_guard<cpen333::process::mutex> lock(pmutex_);
      loc = info_->pidx;
      // increment producer index for next cookie, wrap around if at end
      if ((++info_->pidx) == info_->size) {
        info_->pidx = 0;
      }
      // lock will unlock here as guard runs out of scope
    }

    // ADVANCED: since copying of memory can be expensive (especially with large data types), it is often best
    // not to perform under a lock, if we can get away with it.  Convince yourself that this is safe in this
    // instance (the semaphores prevent overwriting the same cookie).  This lets us copy data in parallel.
    data_[loc] = cookie;  // add cookie to queue

    // let consumer know a cookie is available
    csem_.notify();

  }

  Cookie pop() {

    // wait until cookie available
    csem_.wait();

    size_t loc = 0;  // will store location of cookie to take
    {
      // look at index, protect memory from multiple simultaneous pops
      std::lock_guard<cpen333::process::mutex> lock(cmutex_);
      loc = info_->cidx;

      // increment consumer index for next cookie, wrap around if at end
      if ( (++info_->cidx) == info_->size) {
        info_->cidx = 0;
      }
      // lock will unlock here as guard runs out of scope
    }

    // ADVANCED: since copying of memory can be expensive (especially with large data types), it is often best
    // not to perform under a lock, if we can get away with it.  Convince yourself that this is safe in this
    // instance (the semaphores prevent overwriting the same cookie).  This lets us copy data in parallel.
    Cookie cookie = data_[loc];  // take cookie out

    // let producer know that we are done with the slot
    psem_.notify();

    return cookie;

  }

  size_t size() {
    std::lock_guard<cpen333::process::mutex> lock1(pmutex_);
    std::lock_guard<cpen333::process::mutex> lock2(cmutex_);

    if (info_->pidx < info_->cidx) {
      return info_->size - info_->cidx+info_->pidx;
    }
    return info_->pidx-info_->cidx;
  }

  // Free named resources on POSIX systems which have kernel persistence
  void unlink() {
    memory_.unlink();
    pmutex_.unlink();
    cmutex_.unlink();
    psem_.unlink();
    csem_.unlink();
  }

  static void unlink(const std::string& name) {
    cpen333::process::shared_memory::unlink(name + std::string("_memory_"));
    cpen333::process::mutex::unlink(name + std::string("_pmutex_"));
    cpen333::process::mutex::unlink(name + std::string("_cmutex_"));
    cpen333::process::semaphore::unlink(name + std::string("_psem_"));
    cpen333::process::semaphore::unlink(name + std::string("_csem_"));
  }

};


#endif // COOKIEQUEUE_H

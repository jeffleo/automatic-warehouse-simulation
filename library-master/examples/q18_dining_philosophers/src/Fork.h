#ifndef FORK_H
#define FORK_H

#include <mutex>

// Fork class using a mutex
class Fork {
 private:
  std::mutex mutex_;         // since mutex, person who picks up MUST make sure to put down
  const size_t id_;                // fixed id, useful for ordering

 public:
  Fork(size_t id) : id_(id) {}  // set id

  /**
   * ID label of fork
   * @return id, useful for ordering
   */
  size_t id() const {
    return id_;
  };

  /**
   * Let someone pick up the fork
   */
  void acquire() {
    mutex_.lock();
  }

  /**
   * Release the fork so someone else can pick it up
   */
  void release() {
    mutex_.unlock();
  }

  /**
   * Try to pick up the fork
   * @return true if picked up successfully, false if currently being used
   */
  bool try_acquire() {
    return mutex_.try_lock();
  }
};

#endif //FORK_H

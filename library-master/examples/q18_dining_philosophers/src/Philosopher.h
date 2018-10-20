#ifndef PHILOSOPHER_H
#define PHILOSOPHER_H

#include <string>
#include <utility>
#include <thread>
#include "Fork.h"

class Philosopher {

 private:
  const size_t id_;
  int nforks_;      // number of forks picked up
  Fork* forks_[2];  // pointer to forks

 public:
  Philosopher(size_t id) : id_(id), nforks_(0) {
    forks_[0] = nullptr;
	forks_[1] = nullptr;
  }

  /**
   * BE SURE TO RELEASE ANY FORKS!!!  We can't do it here because they are mutexes, which
   * must be released by the same thread that locked them
   */
  ~Philosopher() {}

  /**
   * Philosopher ID
   * @return id number
   */
  size_t id() const {
    return id_;
  }

  /**
   * Pick up the specified fork, can only hold up to two forks
   * @param fork fork to pick up
   * @return true if picked up, false otherwise
   */
  bool pickup(Fork* fork) {
    if (fork != nullptr && nforks_ < 2) {
      fork->acquire();
      forks_[nforks_] = fork;
      ++nforks_;
      return true;
    }
    return false;
  }

  /**
   * Try to pick up specified fork, can only hold up to two
   * @param fork fork to pick up
   * @return true if picked up, false otherwise
   */
  bool try_pickup(Fork* fork) {
    if (nforks_ < 2 && fork->try_acquire()) {
      forks_[nforks_] = fork;
      ++nforks_;
      return true;
    }
    return false;
  }

  /**
   * Eat for a specified amount of time.  Note that the philosopher does NOT put down the forks automatically.
   * @param ms time in milliseconds
   * @return true if holding 2 forks and eat successful
   */
  bool eat(size_t ms) {
    if (nforks_ != 2) {
      return false;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    return true;
  }

  /**
   * Think for a specified amount of time.
   * @param ms
   */
  void think(size_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  }

  /**
   * Put down any forks we happen to be holding
   */
  void release_forks() {
    if (nforks_ == 2) {
      forks_[1]->release();
      forks_[1] = nullptr;
      --nforks_;
    }

    if (nforks_ == 1) {
      forks_[0]->release();
      forks_[0] = nullptr;
      --nforks_;
    }
  }

};

#endif //PHILOSOPHER_H

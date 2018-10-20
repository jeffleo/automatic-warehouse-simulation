#include <iostream>
#include <cpen333/util.h>

//
// "Resource acquisition is initialization" (RAII) is an important design pattern we will make heavy use of.
//    The idea is to try to guarantee that any resource we acquire will be properly released safely.  In C++, this can
//       be accomplished by creating a special container that acquires/initializes the resource in its constructor, then
//       releases/frees the resource in the destructor.  Destructors are guaranteed to be called when objects run out of
//       scope or are deleted, ensuring the resource is freed even if exceptions occur.
//

/**
 * Creates a safe double array on the heap according to RAII principles
 */
class DoubleArray {
 private:
  double *array_;  // actual pointer to resource

 public:

  // constructor ( i.e. initialization)
  DoubleArray(size_t size) {
    array_ = new double[size];  // acquire resource
  }

  // destructor
  ~DoubleArray() {
    std::cout << "Deleting array" << std::endl;
    delete[] array_;  // release resource
  }

  // override element-access operator so we can use it just like any other kind of array
  double &operator[](size_t idx) {
    return array_[idx];
  }
};


// simple lock mechanism that can lock and unlock
class resource_lock {
 private:
  bool locked_;
 public:
  resource_lock() : locked_(false) {}  // start unlocked
  void lock() { locked_ = true; }      // lock
  void unlock() { locked_ = false; }   // unlock
  bool is_locked() { return locked_; } // check whether mechanism is locked
};


// Class to guard against us accidentally locking a door and throwing away
// the key.  When we are done with a resource, we want to make sure
// it gets unlocked so others can access it.
class lock_guard {
 private:
  resource_lock& lock_;  // reference to lock
 public:

  lock_guard(resource_lock& lk) : lock_(lk) {
    lock_.lock();   // acquire resource, preventing others from using it
  }

  // ensure owned lock is unlocked when we are done
  // this prevents us from accidentally locking everyone else out for good
  ~lock_guard() {
    lock_.unlock();
  }
};

int main() {

  // Memory-managed dynamic array of doubles

  { // brackets used to provide limited scope on variable
    // create array
    DoubleArray safe_array(10);
    for (int i = 0; i < 10; ++i) {
      safe_array[i] = i;
    }

    // print out elements
    std::cout << "Double array values:";
    for (int i = 0; i < 10; ++i) {
      std::cout << " " << safe_array[i];
    }
    std::cout << std::endl;
    // heap memory should be freed here
  }


  // lock with guard to prevent keeping it locked
  resource_lock lock;  // new lock
  {
    std::cout << "state of lock: " << lock.is_locked() << std::endl;

    lock_guard guard(lock);  // acquire resource in initialization
    std::cout << "state of lock: " << lock.is_locked() << std::endl;
  }
  std::cout << "state of lock: " << lock.is_locked() << std::endl;

  cpen333::pause();
  return 0;
}
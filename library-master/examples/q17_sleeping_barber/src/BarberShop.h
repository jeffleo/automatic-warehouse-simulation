#ifndef BARBERSHOP_H
#define BARBERSHOP_H

#define BARBER_SHOP_NAME "SLEEPING_BARBER"
#define BARBER_SHOP_CHAIRS 2

#include <string>

#include <cpen333/process/semaphore.h>
#include <cpen333/process/mutex.h>
#include <cpen333/process/shared_memory.h>

class BarberShop {
 private:
  struct Data {
    int chairs;        // number of free chairs in the barber shop
    int waiters;       // number of people waiting
    int barbers;       // number of barbers waiting
    bool open;         // barshop open
    size_t initialized;   // magic number for checking initialization
  };

  std::string name_ ;
  cpen333::process::semaphore customers_;
  cpen333::process::semaphore barbers_;
  cpen333::process::mutex mutex_;
  cpen333::process::shared_object<Data> data_;

 public:
  BarberShop(const std::string &name, int chairs) :
      name_(name),
      customers_(name + std::string("_sleeping_barbers_customers_"), 0),
      barbers_(name + std::string("_sleeping_barbers_barbers_"), 0),
      mutex_(name + std::string("_sleeping_barbers_mutex_")),
      data_(name + std::string("__sleeping_barbers_data_")) {

    std::lock_guard<cpen333::process::mutex> lock(mutex_);
    if (data_->initialized != 0x98765432) {
      data_->chairs = chairs;
      data_->waiters = 0;
      data_->barbers = 0;
      data_->open = false;
      data_->initialized = 0x98765432;
    }
  }

  void open() {
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    data_->open = true;
  }

  bool isOpened() {
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    return data_->open;
  }

  /**
   * Customers wait here if there's a seat, or barber is free
   * @return true if eventually had haircut, false if left because there were no seats
   */
  bool customerWaitsForHaircut() {
    // protect memory access
    std::unique_lock<decltype(mutex_)> lock(mutex_);

    if (data_->waiters < data_->chairs) {
      // shop is closed
      if (!data_->open) {
        return false;
      }
      ++(data_->waiters);  // one person waiting
      lock.unlock();

      customers_.notify(); // ring bell saying there is at least one customer
      barbers_.wait();     // wait until a barber available

      lock.lock();         // protect data_->open
      return data_->open;  // we may have closed the shop while they were waiting
    }

    // left since waiting room was full
    return false;
  }

  /**
   * Barbers wait here for customers, returns after starting haircut
   * @return true if giving haircut, false if shop closed
   */
  bool barberWaitsForCustomer() {
    std::unique_lock<decltype(mutex_)> lock(mutex_);
    // shop is closed
    if (!data_->open) {
      return false;
    }
    ++(data_->barbers);
    lock.unlock();

    customers_.wait();                  // wait until a customer enters

    lock.lock();
    --(data_->waiters) ;                 // one customer being taken care of
    --(data_->barbers);
    barbers_.notify();                  // barber ready to cut hair

    // cut hair now
    return true;
  }

  /**
   * Release named resources so they can start anew next round
   * @return true if succesfull
   */
  bool close() {

    std::unique_lock<decltype(mutex_)> lock(mutex_);
    data_->open = false;
    // notify everyone
    int waiters = data_->waiters;
    int barbers = data_->barbers;
    lock.unlock();

    // wake up any sleeping barbers with the customer bell
    for (int i=0; i<barbers; ++i) {
      customers_.notify();
    }

    // ring out customers, barber tells them to get out
    for (int i=0; i<waiters; ++i) {
      barbers_.notify();
    }

    // clear data and release
    lock.lock();
    data_->initialized = 0;
    bool b1 = customers_.unlink();
    bool b2 = barbers_.unlink();
    bool b3 = mutex_.unlink();
    bool b4 = data_.unlink();
    return b1 && b2 && b3 && b4;
  }

  /**
   * Release named resources so they can start anew next round
   * @param name barber shop name to release
   * @return true if successful
   */
  static bool unlink(const std::string& name) {
    bool b1 = cpen333::process::semaphore::unlink(name + std::string("_sleeping_barbers_customers_"));
    bool b2 = cpen333::process::semaphore::unlink(name + std::string("_sleeping_barbers_barbers_"));
    bool b3 = cpen333::process::mutex::unlink(name + std::string("_sleeping_barbers_mutex_"));
    bool b4 = cpen333::process::shared_object<Data>::unlink(name + std::string("__sleeping_barbers_data_"));
    return b1 && b2 && b3 && b4;
  }

} ;

#endif //BARBERSHOP_H

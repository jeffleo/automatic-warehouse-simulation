#ifndef BANKACCOUNT_H
#define BANKACCOUNT_H

#include <mutex>
#include <condition_variable>

class BankAccount   {

 private:
  struct BankData {
    double balance ;	// the data to be protected
	BankData(double balance) : balance(balance) {}
  };

  BankData data_;
  std::mutex mutex_;
  std::condition_variable cv_;

 public:

  /**
   * Safely withdraw funds if there is sufficient money in the bank
   * @param amount amount to withdraw
   * @return true if successful, false if not sufficient funds
   */
  bool withdraw (double amount) {
    std::lock_guard<std::mutex> lock(mutex_);

    bool status = false;
    if (data_.balance >= amount) {
      status = true;
      data_.balance -= amount;
      cv_.notify_all();
    }
    return status ;
  }

  /**
   * Add funds to bank account
   * @param amount amount to deposit
   */
  void deposit (double amount) {
    std::lock_guard<std::mutex> lock(mutex_); // protect data
    data_.balance += amount;
    cv_.notify_all();
  }

  /**
   * Retrieve bank balance
   * @return your current savings, treat with care
   */
  double getBalance() {
    std::lock_guard<std::mutex> lock(mutex_);  // this will automatically release mutex after return
    return data_.balance;
  }

  /**
   * Set a new balance in your bank account
   * @param newBalance are you a world-class hacker?  How can you do this without depositing money?
   */
  void setBalance(double new_balance) {
    std::lock_guard<std::mutex> lock(mutex_);
    data_.balance = new_balance;
    cv_.notify_all();
  }

  // allows waiting until there is money in the account
  void waitForMoney() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [&](){ return data_.balance > 0;});
  }

  void waitForBankrupt() {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [&](){ return data_.balance == 0;});
  }

  // constructor and destructor
  BankAccount () :
      data_(0), mutex_(), cv_() {
  }

  ~BankAccount () {}

} ;

#endif //BANKACCOUNT_H

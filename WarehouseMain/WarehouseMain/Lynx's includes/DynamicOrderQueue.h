#ifndef AMAZOOMQUEUE
#define AMAZOOMQUEUE

#include "OrderQueue.h"
#include <deque>
#include <condition_variable>
#include <mutex>

/**
* Dynamically-sized Queue Implementation
*
* Does not block when adding items
*/
class DynamicOrderQueue : public virtual OrderQueue {
	std::deque<Order> buff_;
	std::mutex mutex_;
	std::condition_variable cv_;


public:
	/**
	* Creates the dynamic queue
	*/
	DynamicOrderQueue() :
		buff_(), mutex_(), cv_() {}

	void add(const Order& order) {
		mutex_.lock();
		buff_.push_back(order);
		mutex_.unlock();
		cv_.notify_one();
	}

	Order get() {
		std::unique_lock<std::mutex> uniquemutexref_(mutex_);
		cv_.wait(uniquemutexref_, [&]() { return !buff_.empty(); });

		//cv_.wait(mutex_, [&]() { return buff_.empty(); });	// if using cpen library
		// get first item in queue
		Order out = buff_.front();
		buff_.pop_front();
		uniquemutexref_.unlock();
		return out;
	}
};

#endif

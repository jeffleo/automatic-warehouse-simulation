#ifndef TRUCK_H
#define TRUCK_H

#include "..\..\Warehouse\WarehouseLayout\WarehouseLayout\warehouse_common.h"

#include <cpen333/thread/thread_object.h>
#include <cpen333/process/shared_memory.h>			
#include <cpen333/process/mutex.h>					
#include <cpen333/thread/fifo.h>

#include <cstring>
#include <chrono>
#include <thread>
#include <string>

#include "warehouse_local.h"


class truck : public cpen333::thread::thread_object {
	// display offset for better visibility
	static const int XOFF = 2;
	static const int YOFF = 1;

	TruckData truckdata;
	cpen333::process::mutex mutex_;
	cpen333::process::shared_object<SharedData> memory_;


	int loc_[2];   // location

	unsigned int MaxCapacity = MAXTRUCKCAPACITY;

public:
	unsigned int CurCapacity;						// TO VALIDATE IF CAN READ
	int idx_;

	truck(WarehouseLocation dockloc, int memoryindex, TruckData truckdata) :
		truckdata(truckdata), memory_(WAREHOUSE_MEMORY_NAME + std::to_string(memoryindex)), mutex_(WAREHOUSE_MUTEX_NAME + std::to_string(memoryindex)) {

		loc_[COL_IDX] = dockloc.col;
		loc_[ROW_IDX] = dockloc.row - 1;

		idx_ = truckdata.ID;

		{
			// protect access of number of trucks
			std::lock_guard<decltype(mutex_)> lock(mutex_);
			idx_ = memory_->tinfo.ntrucks;
			memory_->tinfo.ntrucks++;
		}



	}

	void Restock() {
		std::cout << "Starting Restock Truck\n" << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		restockdock.wait();											// acquire dock race against other restock trucks
		std::cout << "Restock Truck Docked\n" << std::endl;
		updateTpos(loc_[COL_IDX], loc_[ROW_IDX]);					// TODO: DRAW TRUCK

		std::unique_lock<decltype(mutex_)> lock(mutex_);
		memory_->tinfo.re_docked = true;							// let layout know restock truck has docked
		lock.unlock();

		std::unique_lock<decltype(mutex_rbuff)> rlock(mutex_rbuff);
		for (int &item : truckdata.itemsvect){
			res_buff.emplace_back(item);							// better version of push_back
		}
		rlock.unlock();

		restock_arrived.notify();									// notify restock dock handler of arrival and buffer ready

		//cv_.wait_until restocktaskid == 0 again, robot will decrement as get restock tasks complete and notify
		std::cout << "Stock dumped, Restock Truck leaving" << std::endl;
	}
	void Delivery() {
		std::cout << "Starting Delivery Truck\n" << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		deliverydock.wait();											// acquire dock race against other restock trucks
		std::cout << "Delivery Truck Docked\n" << std::endl;
		updateTpos(loc_[COL_IDX], loc_[ROW_IDX]);					// TODO: DRAW TRUCK

		std::unique_lock<decltype(mutex_)> lock(mutex_);
		memory_->tinfo.del_docked = true;							// let layout know restock truck has docked
		lock.unlock();

		std::unique_lock<decltype(mutex_dbuff)> dlock(mutex_dbuff);
		for (int &item : truckdata.itemsvect) {
			del_buff.emplace_back(item);							// better version of push_back
		}
		dlock.unlock();

		restock_arrived.notify();									// notify restock dock handler of arrival and buffer ready


		//cv_.wait_until restocktaskid == 0 again, robot will decrement as get restock tasks complete and notify

		std::cout << "Delivery Truck leaving" << std::endl;
		// notify warehouse leaving, and the associated orders are out
	}

	void updateTpos(int &c, int &r) {
		std::lock_guard<decltype(mutex_)> lock(mutex_);
		memory_->tinfo.tloc[idx_][ROW_IDX] = r;
		memory_->tinfo.tloc[idx_][COL_IDX] = c;
	}


	int main() {

		
		if (truckdata.type == TTdelivery)	Delivery();
		else if (truckdata.type == TTrestock)	Restock();
		else {
			std::cout << "Error in truck type" << std::endl;
		}


		return 0;
	}

};


//class res_dockHandler : public cpen333::thread::thread_object {
//	unsigned long restocktaskid = 0;			// counter
//	MazeSolver& shortest;
//
//public :
//	res_dockHandler(MazeSolver &shortest) : shortest(shortest) {};		// std::ref ?
//
//	int main() {
//		while (true) {
//
//			restock_arrived.wait();
//			// restock truck arrived
//
//			Task RestockTask(restocktaskid, TTrestock);
//			GenerateRestockTask(restocktaskid, shortest, RestockTask);
//
//		}
//	}
//};




#endif //TRUCK_H
#ifndef ROBOTS_H
#define ROBOTS_H

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
#include "stock.h"

class robot : public cpen333::thread::thread_object {
	//cpen333::process::shared_object<SharedData> memory_;

	cpen333::process::mutex mutex_;
	SharedData* memory_;
	// local copy of layout
	LayoutInfo linfo_;

	// runner info
	size_t idx_;   // runner index
	int loc_[2];   // current location
	unsigned int MaxCapacity = MAX_ROBOT_CAPACITY;

public:
	unsigned int CurCapacity;						// TO VALIDATE IF CAN READ
	int robotspeed = 50;

	//robot(int startloc[]) : memory_(WAREHOUSE_MEMORY_NAME), mutex_(WAREHOUSE_MUTEX_NAME),
	robot(int startloc[], int memoryindex) : 
		linfo_(), idx_(0), loc_(), mutex_(WAREHOUSE_MUTEX_NAME + std::to_string(memoryIndex)) {

		memory_ = shares.at(memoryIndex);
		//// check if shared memory maze has been intialized yet
		//	{
		//		std::lock_guard<decltype(mutex_)> lock(mutex_);
		//		if (memory_->magic != 1454604) {
		//			memory_->quit = true;
		//		}
		//	}

			// copy maze contents
			linfo_ = memory_->layinfo;

			{
				// protect access of number of runners
				std::lock_guard<decltype(mutex_)> lock(mutex_);
				idx_ = memory_->rinfo.nrunners;
				memory_->rinfo.nrunners++;
			}

			// get/initialize current location
			//loc_[COL_IDX] = memory_->rinfo.rloc[idx_][COL_IDX];
			//loc_[ROW_IDX] = memory_->rinfo.rloc[idx_][ROW_IDX];

			loc_[COL_IDX] = startloc[0];
			loc_[ROW_IDX] = startloc[1];


	}

	/**
	* Solves the maze, taking time between each step so we can actually see progress in the UI
	* @return 1 for success, 0 for failure, -1 to quit
	*/
	int go() {
		if (memory_->quit == 1) {
			std::cout << "Robot quit" << std::endl;
			return -1;
		}
		// current location
		int turns = 0;	// left turns dec, right turns inc
		int dir = 0;
		int c = loc_[COL_IDX];
		int r = loc_[ROW_IDX];
		int c_dir = 1;	// 1, -1 : right, left
		int r_dir = 1;	// 1, -1 : down, up 


		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		//std::cout << "start moving to empty" << std::endl;

		//make sure starting at EMPTY_CHAR 
		// tries moving in every direction until out
		while (linfo_.layout[c][r] == WALL_CHAR || EXIT_CHAR) {
			if (memory_->quit == 1) {
				std::cout << "Robot quit" << std::endl;
				return -1;
			}
			std::cout << "moving out of wall" << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			dir = (dir % 4 + 4) % 4;	// proper modulus operator in C++
			if (!movef(dir, c, r)) dir++;

			std::cout << "moving out of wall char" << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));	// wait before main solving
		std::cout << "Not in wall" << std::endl;


		/*
		// pledge algorithm, prefered right dir
		When wall following, count the number of turns you make,
		e.g. a left turn is -1 and a right turn is 1. Only stop wall following and take your chosen direction
		when the total number of turns you've made is 0, i.e. if you've turned around 360 degrees or more,
		keep wall following until you untwist yourself.
		*/

		std::cout << "Start robot at " << c << "r " << r << std::endl;

		while (linfo_.layout[c][r] != EXIT_CHAR) {
			if (memory_->quit == 1) {
				std::cout << "Robot quit" << std::endl;
				return -1;
			}
			dir = (dir % 4 + 4) % 4;	// proper modulus operator in C++


			//std::cout << "robot running. Turns: " << turns << " dir:" << dir << std::endl;
			if (turns == 0 && dir == 0) { //keep trying while dir right and 0 turns
				while (movef(dir, c, r)) {
					if (memory_->quit == 1) {
						std::cout << "Maze runner quit" << std::endl;
						return -1;
					}
				}

				//obstacle or deadend detected while trying right
				if (movef(dir + 1, c, r)) {	//try turn right by going down
					turns++;
					dir++;
					//std::cout << "blocked, turn right" << std::endl;
				}
				else if (movef(dir - 1, c, r)) {	//try turn left by going up
					turns--;
					dir--;
					//std::cout << "blocked, turn left" << std::endl;
				}
				else {//try turn around by going left
					std::cout << "3" << std::endl;
					if (!movef(dir + 2, c, r)) {
						std::cout << "Stuck no direction possible while turns = 0" << std::endl;
						return 0;
					}
					else {
						turns += 2;	// turn right twice
						dir += 2;	// dir go left 
						//std::cout << "blocked, turn around" << std::endl;

					}
				}
			}
			else if (movef(dir - 1, c, r)) {// not going right or turns!=0, then try turn left
				turns--;
				dir--;
				//std::cout << "turned left" << std::endl;
			}
			else if (movef(dir, c, r)) {// not able turn left but can continue in direction forward
				//std::cout << "move forward" << std::endl;
			}
			else if (movef(dir + 1, c, r)) { //try turn right and go forward
				turns++;
				dir++;
				//std::cout << "Turned right" << std::endl;
			}
			else {	// no other option, turn around
				if (movef(dir + 2, c, r)) {
					turns += 2;	// turn right twice
					dir += 2;	// dir go back
				}
				else {
					std::cout << "Glitch Stuck; no direction possible while turns > 0" << std::endl;
					return 0;
				}
			}

			/// error detection
			if (turns > 1000) {
				std::cout << "Stuck looping left, >1000 turns" << std::endl;
				return 0;
			}
			if (turns < -1000) {
				std::cout << "Stuck looping right, <-1000 turns" << std::endl;
				return 0;
			}
		}
		std::cout << "Robot " << idx_ << " escaped" << std::endl;
		return 1;
	}

	void updateRpos(int &c, int &r) {
		std::lock_guard<decltype(mutex_)> lock(mutex_);
		memory_->rinfo.rloc[idx_][ROW_IDX] = r;
		memory_->rinfo.rloc[idx_][COL_IDX] = c;
	}

	void go2() {
		while (true) {
			Task t = tasks_Q.pop();

			if (t.type == TTdelivery) HandleDelivery(t);
			else if (t.type == TTrestock) HandleRestock(t);
			else {
				//// Just path follow 
				for (WarehouseLocation point : t.path) {
					std::this_thread::sleep_for(std::chrono::milliseconds(robotspeed));
					updateRpos(point.col, point.row);

					if (point.leftrightmiddle != 0) {
						std::cout << " DETECTED INDICATOR, ROBOT DOING WAREHOUSE STUFF" << std::endl;
						std::this_thread::sleep_for(std::chrono::milliseconds(600));
					}
				}
			}

		}
	}

	void HandleDelivery(Task &t) {

		int itemsaquired = false;

		// path follow
		for (WarehouseLocation point : t.path) {
			std::this_thread::sleep_for(std::chrono::milliseconds(robotspeed));
			updateRpos(point.col, point.row);

			if (point.leftrightmiddle == -1 || point.leftrightmiddle == 1) {
				std::cout << "ROBOT: "<< idx_ <<" TAKING STOCK FROM SHELF AT " << point.col+ point.leftrightmiddle <<","<< point.row << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(600));
				// TODO REMOVE FROM STOCK AND PUT INTO ROBOT ITEM LIST
				/*
				UPDATE STOCK INFO MAP GIVEN ITEM ID
					- ITEM INFO
						- QUANTITY--
						- DELETE/POP VECTOR.BACK // TODO: PATH WILL TAKE TO ITEM LOCATIONS AT BACK OF VECTOR 
					
					*/
				//if itemlist is empty
				//	itemsaquired = true;
				// continue;

				
			}
			else if(point.shelflevel == -1) {
				std::cout << "ROBOT: " << idx_ << " EXCHANGING WITH DELIVERY TRUCK " << point.col << "," << point.row << std::endl;

				/* TODO
				Exchange
				report to warehouse task complete
				warehouse appends order corresponding to task into outgoing delivery list

				*/
			}

			//THEN NOTIFY TASKID IS COMPLETE
		}
	}

	void HandleRestock(Task t) {

		int itemsaquired = false;
		for (WarehouseLocation point : t.path) {
			std::this_thread::sleep_for(std::chrono::milliseconds(robotspeed));
			updateRpos(point.col, point.row);


			if ( (point.leftrightmiddle == -1 || point.leftrightmiddle == 1) && itemsaquired) {
				std::cout << "ROBOT: " << idx_ << " RESTOCKING ITEM: "<< t.items.back() <<" AT SHELF LOCATION" << point.col+ point.leftrightmiddle << "," << point.row << std::endl;
				// TODO INSERT ITEM
				/*
				DEQUEUE ITEM_ID
				
				UPDATE STOCK INFO MAP GIVEN ITEM ID
					- ITEM INFO
						-QUANTITY
						-NOT RESERVED, WAREHOUSE LOCATION(THIS LOCATION, SHELF LEVEL, LEFTRIGHT, OCCUPIED
				
				REPEAT WHILE QUEUE !=EMPTY
				*/

				// update stock from list
				if (t.items.size() > 0) {
					StockUpdate(StockInfo_map, t.items.back());
					t.items.pop_back();
					_currentstockquant++;
					std::this_thread::sleep_for(std::chrono::milliseconds(600));		// VISUAL CUE
					
				}
			}
			else if (point.shelflevel == -1) {
				std::cout << "ROBOT: " << idx_ << " EXCHANGING WITH RESTOCK TRUCK " << point.col << "," << point.row << std::endl;

				/* TODO
				WAIT FOR AND...
				DEQUEUE/AQUIRE ITEMS INTO MY OWN QUEUE? OR USE LIST map or vector from truck 
				REPEAT UNTIL CURRENTCAPACITY<MAXCAPACITY
				
				// parse itembasket : vector<pair<Item_ID:int,quantity:int>> OR Map<ItemID: int, Quantity: int> into SINGLE queue of item IDs
				//	 CAN YOU DO FOR EACH WITH A MAP OR KNOW IT QUANTITY AND ID'S WITHOUT PRIOR KNOWLEDGE?



				
				
				//DO BY QUANTITY FASTER
				*/
				//if full
				itemsaquired = true;
			}
			if (restocktaskid > 0) restocktaskid--;

			//THEN NOTIFY TASKID IS COMPLETE 
		}
	}


	/**
	* Checks if we are supposed to quit
	* @return true if memory tells us to quit
	*/
	bool quit() {
		// check if we need to quit
		return memory_->quit;
	}

	bool movef(int dir, int &c, int &r) {
		dir = (dir % 4 + 4) % 4;// proper modulus operator in C++

		std::cout << "trying movef with dir " << dir << " c " << c << " r " << r << std::endl;
		switch (dir) {
		case 0:
			if (!tryright(c, r)) return 0;
			break;
		case 1:
			if (!trydown(c, r)) return 0;

			break;
		case 2:
			if (!tryleft(c, r)) return 0;
			break;
		case 3:
			if (!tryup(c, r)) return 0;
			break;
		}
		return 1;
	}


	bool tryleft(int &c, int &r) {
		if (linfo_.cols > 0 && (linfo_.layout[c - 1][r] == EMPTY_CHAR || linfo_.layout[c - 1][r] == EXIT_CHAR)) { // try going left
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			loc_[COL_IDX] = --c;
			updateRpos(c, r);
			std::cout << "tried left successfully, now at" << " c " << c << " r " << r << std::endl;
			return 1;
		}
		else return 0;
	}

	bool tryright(int &c, int &r) {
		if (linfo_.cols < MAX_LAYOUT_SIZE && (linfo_.layout[c + 1][r] == EMPTY_CHAR || linfo_.layout[c + 1][r] == EXIT_CHAR)) { // try going right
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			loc_[COL_IDX] = ++c;
			updateRpos(c, r);
			std::cout << "tried right successfully, now at" << " c " << c << " r " << r << std::endl;
			return 1;
		}
		else return 0;
	}

	bool tryup(int &c, int &r) {
		if (linfo_.rows > 0 && (linfo_.layout[c][r - 1] == EMPTY_CHAR || linfo_.layout[c][r - 1] == EXIT_CHAR)) { // try going up
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			loc_[ROW_IDX] = --r;
			updateRpos(c, r);
			std::cout << "tried up successfully, now at" << " c " << c << " r " << r << std::endl;
			return 1;
		}
		else return 0;
	}

	bool trydown(int &c, int &r) {
		if (linfo_.rows <MAX_LAYOUT_SIZE && (linfo_.layout[c][r + 1] == EMPTY_CHAR || linfo_.layout[c][r + 1] == EXIT_CHAR)) { // try going down
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
			loc_[ROW_IDX] = ++r;
			updateRpos(c, r);
			std::cout << "tried down successfully, now at" << " c " << c << " r " << r << std::endl;
			return 1;
		}
		else return 0;
	}

	int main() {

		std::cout << "Starting robot\n"<<std::endl;

		//go();			// use this to test sorta random movement in warehosue
		go2();			// use this for path testing from taskQ

		return 0;
	}

};





#endif //ROBOTS_H
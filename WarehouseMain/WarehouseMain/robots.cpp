//#include "..\..\Warehouse\WarehouseLayout\WarehouseLayout\warehouse_common.h"
//
//#include <cpen333/thread/thread_object.h>
//
//
//#include <cpen333/process/shared_memory.h>			//	??
//#include <cpen333/process/mutex.h>					//	??
//
//#include <cstring>
//#include <chrono>
//#include <thread>
//
//class MazeRunner : public cpen333::thread::thread_object  {
//
//	cpen333::process::shared_object<SharedData> memory_;
//	cpen333::process::mutex mutex_;
//
//	// local copy of layout
//	LayoutInfo linfo_;
//
//	// runner info
//	size_t idx_;   // runner index
//	int loc_[2];   // current location
//
//public:
//
//	MazeRunner() : memory_(WAREHOUSE_MEMORY_NAME), mutex_(WAREHOUSE_MUTEX_NAME),
//		linfo_(), idx_(0), loc_() {
//
//		// check if shared memory maze has already been intialized
//		{
//			std::lock_guard<decltype(mutex_)> lock(mutex_);
//			if (memory_->magic != 1454604) {
//				memory_->quit = true;
//			}
//		}
//
//		// copy maze contents
//		linfo_ = memory_->layinfo;
//
//		{
//			// protect access of number of runners
//			std::lock_guard<decltype(mutex_)> lock(mutex_);
//			idx_ = memory_->rinfo.nrunners;
//			memory_->rinfo.nrunners++;
//		}
//
//		// get/initialize current location
//		loc_[COL_IDX] = memory_->rinfo.rloc[idx_][COL_IDX];
//		loc_[ROW_IDX] = memory_->rinfo.rloc[idx_][ROW_IDX];
//
//	}
//
//	/**
//	* Solves the maze, taking time between each step so we can actually see progress in the UI
//	* @return 1 for success, 0 for failure, -1 to quit
//	*/
//	int go() {
//		if (memory_->quit == 1) {
//			std::cout << "Robot quit" << std::endl;
//			return -1;
//		}
//		// current location
//		int turns = 0;	// left turns dec, right turns inc
//		int dir = 0;
//		int c = loc_[COL_IDX];
//		int r = loc_[ROW_IDX];
//		int c_dir=1;	// 1, -1 : right, left
//		int r_dir=1;	// 1, -1 : down, up 
//
//
//		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
//		//make sure starting at EMPTY_CHAR 
//		// tries moving in every direction until out
//		while (linfo_.layout[c][r] == WALL_CHAR) {
//			if (memory_->quit == 1) {
//				std::cout << "Maze runner quit" << std::endl;
//				return -1;
//			}
//			std::this_thread::sleep_for(std::chrono::milliseconds(100));
//			//if (minfo_.cols < MAX_MAZE_SIZE) {//try right
//			//	c += c_dir;
//			//}
//			// else if (minfo_.cols > 0) { // try left
//			//	c -= c_dir;;
//			//} else if (minfo_.rows < MAX_MAZE_SIZE) { //try down
//			//	r += r_dir;
//			//} else if (minfo_.rows > 0) { // try up
//			//	r -= r_dir;
//			//}
//			dir = dir % 4;
//			if (!movef(dir, c, r)) dir++;	
//		}
//		std::this_thread::sleep_for(std::chrono::milliseconds(1000));	// wait before main solving
//
//
//		/*
//		///////////////////pledge algorithm, prefered right dir
//		When wall following, count the number of turns you make,
//		e.g. a left turn is -1 and a right turn is 1. Only stop wall following and take your chosen direction
//		when the total number of turns you've made is 0, i.e. if you've turned around 360 degrees or more,
//		keep wall following until you untwist yourself.
//
//		tryleft
//		do while tryleft
//		else trydown, inc turns,
//
//		if (direction_attempt_state[circular_state_index(int dir)])
//		else iterate circular_state_index(int dir) and turns+=dir and netturns++;
//
//		if turns>4 ??
//		*/
//
//		while (linfo_.layout[c][r] != EXIT_CHAR || SHELF_CHAR || DELIVERYDOCK_CHAR || RESTOCKDOCK_CHAR) {
//			if (memory_->quit == 1) {
//				std::cout << "Maze runner quit" << std::endl;
//				return -1;
//			}
//			dir = dir % 4;
//
//			if (turns == 0 && dir == 0) { //keep trying while dir right and 0 turns
//				while (movef(dir, c, r));
//				//obstacle or deadend detected while trying right
//				if (movef(dir + 1, c, r)) {	//try turn right by going down
//					turns++;
//					dir++;
//				}
//				else if (movef(dir - 1, c, r)) {	//try turn left by going up
//					turns--;
//					dir--;
//				}
//				else {//try turn around by going left
//					if (!movef(dir + 2, c, r)) {
//						std::cout << "Stuck no direction possible while turns=0" << std::endl;
//						return 0;
//					}
//					else {
//						turns += 2;	// turn right twice
//						dir += 2;	// dir go left 
//
//					}
//				}
//			}
//			else if (tryleft(c, r)) {// not going right or turns!=0, then try go left
//				turns--;
//				dir--;
//			}
//			else if (movef(dir, c, r));	// not able for left but can continue in direction forward
//			else {
//				if (movef(dir + 1, c, r)) {	//try turn right and go forward
//					turns++;
//					dir++;
//				}
//				else {
//					std::cout << "Stuck no direction possible while turns>0" << std::endl;
//					return 0;
//				}
//			}
//
//			if (turns > 1000) {
//				std::cout << "Stuck looping left, >1000 turns" << std::endl;
//				return 0;
//			}
//			if (turns < -1000) {
//				std::cout << "Stuck looping right, <-1000 turns" << std::endl;
//				return 0;
//			}
//		}
//		return 1;
//	}
//
//	void updateRpos(int c, int r) {
//		memory_->rinfo.rloc[idx_][ROW_IDX] = r;
//		memory_->rinfo.rloc[idx_][COL_IDX] = c;
//	}
//
//	/**
//	* Checks if we are supposed to quit
//	* @return true if memory tells us to quit
//	*/
//	bool quit() {
//		// check if we need to quit
//		return memory_->quit;
//	}
//	
//	bool movef(int dir, int c, int r) {
//		dir = dir % 4;
//		switch (dir) {
//		case 0:
//			if (!tryright(c, r)) return 0;
//			
//			break;
//		case 1:	
//			if (!trydown(c, r)) return 0;
//			
//			break;
//		case 2:	
//			if (!tryleft(c, r)) return 0;
//			break;
//		case 3:	
//			if (!tryup(c, r)) return 0;
//			break;
//		}
//		return 1;
//	}
//
//	
//	bool tryleft(int c, int r) {
//		if (linfo_.cols > 0 && linfo_.layout[c-1][r] == EMPTY_CHAR) { // try going left
//			std::this_thread::sleep_for(std::chrono::milliseconds(100));
//			loc_[COL_IDX] = --c;
//			updateRpos(c, r);
//			return 1;
//		}
//		else return 0;
//	}
//
//	bool tryright(int c, int r) {
//		if (linfo_.cols < MAX_LAYOUT_SIZE && linfo_.layout[c+1][r] == EMPTY_CHAR) { // try going right
//			std::this_thread::sleep_for(std::chrono::milliseconds(100));
//			loc_[COL_IDX] = ++c;
//			updateRpos(c, r);
//			return 1;
//		}
//		else return 0;
//	}
//
//	bool tryup(int c, int r) {
//		if (linfo_.rows > 0 && linfo_.layout[c][r-1] == EMPTY_CHAR) { // try going up
//			std::this_thread::sleep_for(std::chrono::milliseconds(100));
//			loc_[ROW_IDX] = --r;
//			updateRpos(c, r);
//			return 1;
//		}
//		else return 0;
//	}
//
//	bool trydown(int c, int r) {
//		if (linfo_.rows <MAX_LAYOUT_SIZE  && linfo_.layout[c][r+1] == EMPTY_CHAR) { // try going down
//			std::this_thread::sleep_for(std::chrono::milliseconds(100));
//			loc_[ROW_IDX] = ++r;
//			updateRpos(c, r);
//			return 1;
//		}
//		else return 0;
//	}
//
//	int main() {
//
//		//MazeRunner runner;
//		//runner.go();
//
//		go();
//
//		return 0;
//	}
//
//};
//

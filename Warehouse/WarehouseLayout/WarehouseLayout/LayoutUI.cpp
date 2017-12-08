#include "LayoutUI.h"


//* Handles all drawing/memory synchronization for the
//* User Interface process
//*
//*/

LayoutUI::LayoutUI() : display_(), memory_(LAYOUT_MEMORY_NAME), mutex_(LAYOUT_MUTEX_NAME) {

	// clear display and hide cursor
	display_.clear_all();
	display_.set_cursor_visible(false);

	// initialize last known runner positions
	for (size_t i = 0; i<MAX_BOTS; ++i) {
		lastpos_[i][COL_IDX] = -1;
		lastpos_[i][ROW_IDX] = -1;
	}

	// initialize exit location
	exit_[COL_IDX] = -1;
	exit_[ROW_IDX] = -1;

	//{	// check magic key to determine if shared memory has been generated previously
	//	if (memory_->magic != 1454604) {
	//		std::lock_guard<decltype(mutex_)> lock(mutex_);
	//		memory_->quit = true;
	//	}
	//}

	{
		std::lock_guard<decltype(mutex_)> lock(mutex_);				// LOCK HERE IN CASE MULTIPLE MAZES&UI RUNNING
																	//copy layout cols and rows count
		int cols = memory_->layinfo.cols;
		int rows = memory_->layinfo.rows;

		for (int i = 0; i < cols; ++i) {
			for (int j = 0; j < rows; ++j) {
				if (memory_->layinfo.layout[i][j] == EXIT_CHAR) {
					exit_[COL_IDX] = i;
					exit_[ROW_IDX] = j;
				}
			}
		}
	}
	//===========================================================
	// SEARCH MAZE FOR EXIT LOCATION -> Done
	//===========================================================

}

/**
* Draws the layout itself
*/
void LayoutUI::draw_layout() {
									//create pointers 
	LayoutInfo& linfo = memory_->layinfo;
	RunnerInfo& rinfo = memory_->rinfo;

	// clear display
	display_.clear_display();

	// draw layout
	for (int r = 0; r < linfo.rows; ++r) {
		display_.set_cursor_position(YOFF + r, XOFF);
		for (int c = 0; c < linfo.cols; ++c) {
			char ch = linfo.layout[c][r];
			if (ch == WALL_CHAR) {
				std::printf("%c", WALL);
			}
			else if (ch == EXIT_CHAR) {
				std::printf("%c", EXIT);
			}
			else if (ch == DELIVERYDOCK_CHAR) {
				std::printf("%c", DDOCK);
			}
			else if (ch == RESTOCKDOCK_CHAR) {
				std::printf("%c", RDOCK);
			}
			else if (ch == SHELF_CHAR) {
				std::printf("%c", SHELF);
			}
			//else if (ch == UP_TRAFFIC_CHAR) {
			//	std::printf("%c", UP);
			//}
			//else if (ch == DOWN_TRAFFIC_CHAR) {
			//	std::printf("%c", DOWN);
			//}
			//else if (ch == RIGHT_TRAFFIC_CHAR) {
			//	std::printf("%c", RIGHT);
			//}
			//else if (ch == LEFT_TRAFFIC_CHAR) {
			//	std::printf("%c", LEFT);
			//}
			else {
				std::printf("%c", EMPTY_CHAR);
			}
		}
	}
}

/**
* Draws all runners in the layout
*/
void LayoutUI::draw_runners() {

	RunnerInfo& rinfo = memory_->rinfo;

	// draw all runner locations
	for (size_t i = 0; i<rinfo.nrunners; ++i) {
		char me = 'A' + i;
		int newr = rinfo.rloc[i][ROW_IDX];
		int newc = rinfo.rloc[i][COL_IDX];

		// if not already at the exit...
		if (newc != exit_[COL_IDX] || newr != exit_[ROW_IDX]) {
			if (newc != lastpos_[i][COL_IDX]
				|| newr != lastpos_[i][ROW_IDX]) {

				// zero out last spot and update known location
				display_.set_cursor_position(YOFF + lastpos_[i][ROW_IDX], XOFF + lastpos_[i][COL_IDX]);
				std::printf("%c", EMPTY_CHAR);
				lastpos_[i][COL_IDX] = newc;
				lastpos_[i][ROW_IDX] = newr;
			}

			// print runner at new location
			display_.set_cursor_position(YOFF + newr, XOFF + newc);
			std::printf("%c", me);
		}
		else {

			// erase old position if now finished
			if (lastpos_[i][COL_IDX] != exit_[COL_IDX] || lastpos_[i][ROW_IDX] != exit_[ROW_IDX]) {
				display_.set_cursor_position(YOFF + lastpos_[i][ROW_IDX], XOFF + lastpos_[i][COL_IDX]);
				std::printf("%c", EMPTY_CHAR);
				lastpos_[i][COL_IDX] = newc;
				lastpos_[i][ROW_IDX] = newr;

				// display a completion message
				display_.set_cursor_position(YOFF, XOFF + memory_->layinfo.cols + 2);
				std::printf("runner %c escaped!!", me);
			}
		}
	}
}



/**
* Checks if we are supposed to quit
* @return true if memory tells us to quit
*/
bool LayoutUI::quit() {
	// check if we need to quit
	return memory_->quit;
}

LayoutUI::~LayoutUI() {
	// reset console settings
	display_.clear_all();
	display_.reset();
}












//
//class LayoutUI {
//	// display offset for better visibility
//	static const int XOFF = 2;
//	static const int YOFF = 1;
//
//	cpen333::console display_;
//	cpen333::process::shared_object<SharedData> memory_;
//	cpen333::process::mutex mutex_;
//
//	// previous positions of runners
//	int lastpos_[MAX_BOTS][2];
//	int exit_[2];   // exit location
//
//public:
//
//	LayoutUI() : display_(), memory_(LAYOUT_MEMORY_NAME), mutex_(LAYOUT_MUTEX_NAME) {
//
//		// clear display and hide cursor
//		display_.clear_all();
//		display_.set_cursor_visible(false);
//
//		// initialize last known runner positions
//		for (size_t i = 0; i<MAX_BOTS; ++i) {
//			lastpos_[i][COL_IDX] = -1;
//			lastpos_[i][ROW_IDX] = -1;
//		}
//
//		// initialize exit location
//		exit_[COL_IDX] = -1;
//		exit_[ROW_IDX] = -1;
//
//		//{	// check magic key to determine if shared memory has been generated previously
//		//	if (memory_->magic != 1454604) {
//		//		std::lock_guard<decltype(mutex_)> lock(mutex_);
//		//		memory_->quit = true;
//		//	}
//		//}
//
//		{
//			std::lock_guard<decltype(mutex_)> lock(mutex_);				// LOCK HERE IN CASE MULTIPLE MAZES&UI RUNNING
//																		//copy layout cols and rows count
//			int cols = memory_->layinfo.cols;
//			int rows = memory_->layinfo.rows;
//
//			for (int i = 0; i < cols; ++i) {
//				for (int j = 0; j < rows; ++j) {
//					if (memory_->layinfo.layout[i][j] == EXIT_CHAR) {
//						exit_[COL_IDX] = i;
//						exit_[ROW_IDX] = j;
//					}
//				}
//			}
//		}
//		//===========================================================
//		// SEARCH MAZE FOR EXIT LOCATION -> Done
//		//===========================================================
//
//	}
//
//	/**
//	* Draws the layout itself
//	*/
//	void draw_layout() {
//		static const char WALL = 219;  // WALL character
//		static const char EXIT = 176;  // EXIT character
//
//									   //create pointers 
//		LayoutInfo& linfo = memory_->layinfo;
//		RunnerInfo& rinfo = memory_->rinfo;
//
//		// clear display
//		display_.clear_display();
//
//		// draw layout
//		for (int r = 0; r < linfo.rows; ++r) {
//			display_.set_cursor_position(YOFF + r, XOFF);
//			for (int c = 0; c < linfo.cols; ++c) {
//				char ch = linfo.layout[c][r];
//				if (ch == WALL_CHAR) {
//					std::printf("%c", WALL);
//				}
//				else if (ch == EXIT_CHAR) {
//					std::printf("%c", EXIT);
//				}
//				else {
//					std::printf("%c", EMPTY_CHAR);
//				}
//			}
//		}
//	}
//
//	/**
//	* Draws all runners in the layout
//	*/
//	void draw_runners() {
//
//		RunnerInfo& rinfo = memory_->rinfo;
//
//		// draw all runner locations
//		for (size_t i = 0; i<rinfo.nrunners; ++i) {
//			char me = 'A' + i;
//			int newr = rinfo.rloc[i][ROW_IDX];
//			int newc = rinfo.rloc[i][COL_IDX];
//
//			// if not already at the exit...
//			if (newc != exit_[COL_IDX] || newr != exit_[ROW_IDX]) {
//				if (newc != lastpos_[i][COL_IDX]
//					|| newr != lastpos_[i][ROW_IDX]) {
//
//					// zero out last spot and update known location
//					display_.set_cursor_position(YOFF + lastpos_[i][ROW_IDX], XOFF + lastpos_[i][COL_IDX]);
//					std::printf("%c", EMPTY_CHAR);
//					lastpos_[i][COL_IDX] = newc;
//					lastpos_[i][ROW_IDX] = newr;
//				}
//
//				// print runner at new location
//				display_.set_cursor_position(YOFF + newr, XOFF + newc);
//				std::printf("%c", me);
//			}
//			else {
//
//				// erase old position if now finished
//				if (lastpos_[i][COL_IDX] != exit_[COL_IDX] || lastpos_[i][ROW_IDX] != exit_[ROW_IDX]) {
//					display_.set_cursor_position(YOFF + lastpos_[i][ROW_IDX], XOFF + lastpos_[i][COL_IDX]);
//					std::printf("%c", EMPTY_CHAR);
//					lastpos_[i][COL_IDX] = newc;
//					lastpos_[i][ROW_IDX] = newr;
//
//					// display a completion message
//					display_.set_cursor_position(YOFF, XOFF + memory_->layinfo.cols + 2);
//					std::printf("runner %c escaped!!", me);
//				}
//			}
//		}
//	}
//
//
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
//	~LayoutUI() {
//		// reset console settings
//		display_.clear_all();
//		display_.reset();
//	}
//};

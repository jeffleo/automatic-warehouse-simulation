#include "LayoutUI.h"


//* Handles all drawing/memory synchronization for the
//* User Interface process
//*
//*/

LayoutUI::LayoutUI(int memoryindex) : display_(), memory_(WAREHOUSE_MEMORY_NAME + std::to_string(memoryindex)), mutex_(WAREHOUSE_MUTEX_NAME + std::to_string(memoryindex)) {

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
	//int shelfcounter_r = 0;
	//int shelfcounter_c = 0;

	////int lastrow[MAX_LAYOUT_SIZE];
	////int lastcol[MAX_LAYOUT_SIZE];

	//int lastrow=0;
	//int lastcol=0;

	for (int r = 0; r < linfo.rows; ++r) {

		display_.set_cursor_position(YOFF + r, XOFF);
		for (int c = 0; c < linfo.cols; ++c) {
			char ch = linfo.layout[c][r];
			if (ch == WALL_CHAR) {
				std::printf("%c", WALL);
				linfo.access[c][r] = 1;
				linfo.acc.at(c).at(r) = 0;
			}
			else if (ch == EXIT_CHAR) {
				std::printf("%c", EXIT);
				linfo.access[c][r] = 1;
				linfo.acc.at(c).at(r) = 0;
			}
			else if (ch == DELIVERYDOCK_CHAR) {
				std::printf("%c", DDOCK);
				linfo.access[c][r] = 0;
				linfo.acc.at(c).at(r) = 0;
			}
			else if (ch == RESTOCKDOCK_CHAR) {
				std::printf("%c", RDOCK);
				linfo.access[c][r] = 0;
				linfo.acc.at(c).at(r) = 0;
			}
			else if (ch == SHELF_CHAR) {
				std::printf("%c", SHELF);
				linfo.access[c][r] = 1;
				linfo.acc.at(c).at(r) = 1;

				//// maps and counts shelves to rows and columns according the layout
				//if (r != lastrow) {
				//	shelfcounter_r++;

				//	if (r > lastrow)
				//		linfo.shelves_rows++;	// counts only till end 

				//	lastrow = r;
				//}

				//// always different col as we cycle through cols
				//if (c != lastcol ) {

				//	linfo.occupied = false;
				//	//shelfmap[ Point(shelfcounter_r, shelfcounter_c++)] = &linfo;
				//	//std::pair<int, int> key = std::make_pair(shelfcounter_r, shelfcounter_c++);
				//	//shelfmap.insert({ key, &linfo });

				//	//shelfmap[std::make_pair(shelfcounter_r, shelfcounter_c++)] = &linfo;


				//	//shelfmap[std::pair<int, int>(shelfcounter_r, shelfcounter_c++)] = &linfo;
				//	//shelfmap[shelfcounter_c++] = &linfo;
				//	//shelfmapcopy[shelfcounter_c++] = 1;


				//	if (c > lastcol)
				//		linfo.shelves_cols++;		// counts only till end 

				//	lastcol=c;
				//}

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
				linfo.access[c][r] = 0;
				linfo.acc.at(c).at(r) = 0;
			}


			//linfo.leftrightmiddle = 0;
			//shelfcounter_c = 0;		// new column
		}
		//shelfcounter_r = 0;		// new row
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
			if (newr == 0 && newc == 0) {
				newr = 1; newc = 1;					// keeps it from starting at corner
			}
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
				display_.set_cursor_position(YOFF + i, XOFF + memory_->layinfo.cols + 2);
				std::printf("runner %c escaped!!", me);
			}
		}

		//draw_path(i);
	}
}


void LayoutUI::draw_path(int idx) {
	
	RRbot& rob = memory_->rrbot_info[idx];

	std::unique_lock<decltype(mutex_)> lock(mutex_);
	bool draw = rob.drawpath;
	lock.unlock();


	if (draw) {

		//for (int r = 0; r < MAX_LAYOUT_SIZE; ++r) 	{	
		//	for (int c = 0; c < MAX_LAYOUT_SIZE; ++c) {
		//		std::unique_lock<decltype(mutex_)> lock(mutex_);
		//		int ch = rob.path[r][c];
		//		lock.unlock();
		//		if (ch == 254) {
		//			display_.set_cursor_position(YOFF + r, XOFF + c);
		//			std::printf("%c", 176);		
		//		}
		//	}
		//}


		std::lock_guard<decltype(mutex_)> lock(mutex_);
		rob.drawpath= false;
	}
}

void LayoutUI::draw_stock() {
		//todo
	/*
	update stock quantity at left/right of each shelf location periodically, same rate as robots
	// shelf map?

	*/

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






#include "layout_common.h"
#include "LayoutUI.h"

#include <string>
#include <fstream>
#include <thread>
#include <random>
#include <cpen333/process/mutex.h>
#include <cpen333/process/shared_memory.h>
//#include <cpen333/process/subprocess.h>

#include <stdlib.h>

#include <cstdio>
#include <cpen333/console.h>
#include <thread>
#include <chrono>


/*
To run:
Run warehouse, Leave the window open so you can quit all the processes later by pressing the ENTER key.
*/


/**
* Reads a layout from a filename and populates the layout
* @param filename file to load layout from
* @param minfo layout info to populate
*/
bool load_layout(const std::string& filename, LayoutInfo& layinfo) {

	// initialize number of rows and columns
	layinfo.rows = 0;
	layinfo.cols = 0;

	std::ifstream fin(filename);
	std::string line;

	// read layout file
	if (fin.is_open()) {
		int row = 0;  // zeroeth row
		while (std::getline(fin, line)) {
			int cols = line.length();
			if (cols > 0) {
				// longest row defines columns
				if (cols > layinfo.cols) {
					layinfo.cols = cols;
				}
				for (size_t col = 0; col<cols; ++col) {
					layinfo.layout[col][row] = line[col];
					//layinfo.visits[col][row] = 0;					// initialize visits to 0
				}
				++row;
			}
		}
		layinfo.rows = row;
		fin.close();
		return true;
	}
	else
	{
		return false;
	}

}

int main(int argc, char* argv[]) {
	

	std::string layout;
	// read desired layout from command-line, else default to local layout
	if (argc > 1) {
		layout = argv[1];
	}
	else {
		layout = "data/layout3.txt";
	}

	////Handling memory layout/organization must be done manually
	//// init shared memory class, 
	//cpen333::process::shared_memory memory(LAYOUT_MEMORY_NAME, sizeof(SharedData));
	//
	//SharedData* shared = (SharedData*)memory.get();

	//alternative
	cpen333::process::mutex mutex_(LAYOUT_MUTEX_NAME);
	cpen333::process::shared_object<SharedData> memory(LAYOUT_MEMORY_NAME);
	SharedData* shared = (SharedData*)memory.get();;	// get pointer to shared memory

	//init
	bool load_success;
	mutex_.lock();					
	load_success = load_layout(layout, shared->layinfo);	//initializing layout
	shared->magic = 1454604;
	mutex_.unlock();
	
	if (load_success) {
		LayoutUI ui;
		ui.draw_layout();
		std::cout << "Loaded Layout file: " << layout << std::endl;

		// continue looping until main program has quit
		while (!ui.quit()) {
			//ui.draw_runners();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			//std::cout << "drawing runners" << std::endl;
		}

	}
	else {
		std::cout << "Load layout failed" << std::endl;
		std::cin.get();
	}

	//cpen333::process::subprocess layoutUI("./layout_UI", true, true);		//detached, start now

	//std::cout << "Press ENTER to quit." << std::endl;
	//std::cin.get();

	//===============================================================
	//  INFORM OTHER PROCESSES TO QUIT
	std::lock_guard<decltype(mutex_)> lock(mutex_);
	shared->quit = true;
	shared->magic = 0;

	return 0;
}
#include "warehouse_common.h"
#include "LayoutUI.h"

#include <string>
#include <fstream>
#include <thread>
#include <random>
#include <cpen333/process/mutex.h>
#include <cpen333/process/shared_memory.h>

#include <stdlib.h>
#include <cstdio>
#include <cpen333/console.h>
#include <thread>
#include <chrono>

#include <string>

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
		std::cout << "attempted to load " << filename << std::endl;
		return false;
	}

}

int main(int argc, char* argv[]) {
	std::cout << "Started layout process" << std::endl;

	std::string layout;
	// read desired layout from command-line, else default to local layout
	if (argc > 1) {
		layout = argv[1];
	}
	else {
		//layout = "data/layout3.txt";
		//layout = "data/layout3_simple.txt";
		layout = "data/layout4.txt";
	}
	
// PREAQUIRE MEMORY NAMES (ACCESSIBLE ONLY WITHIN THIS SCOPE)
	std::vector<SharedData*> shares;
		 
	int i = 0;
	std::string memoryname = WAREHOUSE_MEMORY_NAME + std::to_string(i++);
	
	cpen333::process::shared_object<SharedData> memory(memoryname);
	shares.push_back((SharedData*)memory.get());
	memory.unlink();																	// unlink for future use of named resource, optional


	memoryname = WAREHOUSE_MEMORY_NAME+ std::to_string(i++);
	cpen333::process::shared_object<SharedData> memory2(memoryname);
	shares.push_back((SharedData*)memory2.get());
	memory2.unlink();																	// unlink for future use of named resource, optional

	memoryname = WAREHOUSE_MEMORY_NAME + std::to_string(i++);
	cpen333::process::shared_object<SharedData> memory3(memoryname);
	shares.push_back((SharedData*)memory3.get());
	memory3.unlink();																	// unlink for future use of named resource, optional

	memoryname = WAREHOUSE_MEMORY_NAME + std::to_string(i++);
	cpen333::process::shared_object<SharedData> memory4(memoryname);	
	shares.push_back((SharedData*)memory4.get());
	memory4.unlink();																	// unlink for future use of named resource, optional
	
	int MAX_NUM_WAREHOUSES = i;														// Max 4 warehousess, repeat for more
// HANDOUT UNIQUE MEMORY NAME
	SharedData* shared = (SharedData*)memory.get();	// get pointer to shared memory
	//int magic = shared->magic;		// UNPROTECTED as no one else will write to magic

	int magic = 0;
	bool unique = false;
	i = 0;
	int memoryIndex =0;
	while (i < MAX_NUM_WAREHOUSES && !unique) {
		std::cout << "Initializing unique layout: " << i << std::endl;
		//std::cout << "Memory name: " << memoryname << std::endl;
		shared = shares.at(i);												// get pointer to shared memory
		magic = shared->magic;
		//std::cout << "unique magic: " << magic << std::endl;
		if (magic != 1454604 + i) {
			memoryIndex = i;
			unique = true;
			shared->magic = 1454604 + i;														//not init yet so aquire this magic 
																								// UNPROTECTED as no one else will write to magic
		}
		i++;
		
	}
	
	if (!unique)
	{
		std::cout << "Couldn't aquire unique named resource" << std::endl;
		cpen333::pause();
		return 0;
	}
	else {
		std::cout << "Unique magic aquired: " << shared->magic << std::endl;
	}
	

	cpen333::process::mutex mutex_(WAREHOUSE_MUTEX_NAME + std::to_string(memoryIndex));

	//cpen333::pause();
	//END OF UNIQUE MEMORY AQUISISTION
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// init layout
	bool load_success;
	mutex_.lock();
	load_success = load_layout(layout, shared->layinfo);	//initializing layout
	//shared->drawpath = false;								// temporary
	mutex_.unlock();
	
	cpen333::console display_;
	display_.set_cursor_visible(false);

	if (load_success) {
		LayoutUI ui(memoryIndex);
		ui.draw_layout();

		std::cout << "   Loaded Layout file: " << layout << std::endl;
		std::cout << "   Warehouse Number " << memoryIndex << std::endl;
		// continue looping until main program has quit
		//unsigned int displaycounter=0;
		while (!ui.quit()) {
			ui.draw_runners();
			//ui.draw_path(0);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));			// max runner speed
			//display_.set_cursor_position(((displaycounter++ % 5) + 5) % 5, 30);
			//std::cout << "  drawing runners "<< displaycounter << std::endl;
		}
	}
	else {
		std::cout << "Load layout failed" << std::endl;
		std::cin.get();
	}


	//std::cout << "Press ENTER to quit." << std::endl;
	//std::cin.get();

	//===============================================================
	//  INFORM OTHER PROCESSES TO QUIT
	std::lock_guard<decltype(mutex_)> lock(mutex_);
	shared->quit = true;
	shared->magic = 0;

	return 0;
}


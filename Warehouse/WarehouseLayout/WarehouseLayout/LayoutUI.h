#pragma once
#ifndef LAYOUTUI_H
#define LAYOUTUI_H
#include "warehouse_common.h"
#include <cstdio>
#include <cpen333/console.h>
#include <thread>
#include <chrono>
#include <cpen333/process/mutex.h>
#include <cpen333/process/shared_memory.h>
#include <string>
#include <cpen333\thread\condition.h>

class LayoutUI {
	

	// Ref: http://www.theasciicode.com.ar/
	static const char WALL = 219;  // WALL character 
	static const char EXIT = 176;  // EXIT character 

	static const char DDOCK = 194;	// ┬
	static const char RDOCK = 193;	// ┴
	static const char SHELF = 186;	// ║

	// traffic
	static const char UP = 24;		// ↑  , 30 ▲ 
	static const char DOWN = 25;	// ↓  , 31 ▼
	static const char LEFT = 26;	// →  , 16 ►
	static const char RIGHT = 27;	// ←  , 17 ◄

	static const char BLOCK = 254;   // ■

	// shelf stock indicator


	// display offset for better visibility
	static const int XOFF = 2;
	static const int YOFF = 1;
	
	cpen333::console display_;
	cpen333::process::shared_object<SharedData> memory_;
	cpen333::process::mutex mutex_;
	
	// previous positions of runners
	int lastpos_[MAX_BOTS][2];
	int exit_[2];   // exit location
public:

	LayoutUI(int memoryindex);
	~LayoutUI();
	void draw_layout();
	void draw_runners();
	void draw_path(int idx);
	void draw_stock();
	bool quit();



};
#endif //LAYOUTUI_H
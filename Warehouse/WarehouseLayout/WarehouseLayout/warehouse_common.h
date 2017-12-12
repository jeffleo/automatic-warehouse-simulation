#ifndef WAREHOUSE_COMMON_H
#define WAREHOUSE_COMMON_H

#define WAREHOUSE_MEMORY_NAME "warehouse_memory"
#define WAREHOUSE_MUTEX_NAME "warehosue_mutex"


#define WALL_CHAR 'X'
#define EMPTY_CHAR ' '
#define EXIT_CHAR 'E'
#define DELIVERYDOCK_CHAR 'D'
#define RESTOCKDOCK_CHAR 'R'
#define SHELF_CHAR 'S'

#define UP_TRAFFIC_CHAR '^'
#define DOWN_TRAFFIC_CHAR 'V'
#define RIGHT_TRAFFIC_CHAR '>'
#define LEFT_TRAFFIC_CHAR '<'

#define COL_IDX 0
#define ROW_IDX 1

#define MAX_LAYOUT_SIZE 50
#define MAX_SHELF_SIZE 5
#define MAX_BOTS 50
#define POISON 123123
#define MAX_TASKS 100
#define MAX_ROBOT_CAPACITY 40		// kg

#define MAXWAREHOUSESTOCK 1000		// total sum of item quantitys is up to this 
#define MAXITEMTYPES 30
#define MAXTRUCKCAPACITY	50		//kg


#include <array>

#include <cpen333/process/mutex.h>
#include <cpen333/process/shared_memory.h>
#include <cpen333/process/subprocess.h>

struct WarehouseLocation {
	// int type;
	int row;
	int col;

	// allocated while stocking
	bool occupied[2][MAX_SHELF_SIZE];					// indication for empty/not empty shelf
	int shelflevel;					// for storing item location vertically, IF -1 this is a truck location
	int leftrightmiddle;			// -1 for left, 1 for right, 0 for middle (aka not shelf)		
									// when storing items, decide whether or not left or right shelf

	WarehouseLocation() : leftrightmiddle(0), row(), col(), shelflevel(0) { 
		for (int i = 0; i < 2; ++i) {
			for (int j = 0; j < MAX_SHELF_SIZE; ++j) {
				occupied[i][j] = false;
			}
		}
	}
	WarehouseLocation(int row, int col) : leftrightmiddle(0), shelflevel(0), row(row), col(col) {// for locating basic points
		for (int i = 0; i < 2; ++i) {
			for (int j = 0; j < MAX_SHELF_SIZE; ++j) {
				occupied[i][j] = false;
			}
		}
	}		

	// shouldn't use, should already be preallocated when shelf mapping
	//WarehouseLocation(int row, int col, int leftrightmiddle, int shelflevel) :			// for locating item stocking??; NOTE DEFAULT occupied = true
	//	leftrightmiddle(leftrightmiddle), row(row), col(col), shelflevel(shelflevel) {
	//	
	//	occupied[ceil(leftrightmiddle)][shelflevel] = true;

	//}		
};

struct StockLocation {

};

struct LayoutInfo {
	int rows;           // rows in layout
	int cols;           // columns in layout
	char layout[MAX_LAYOUT_SIZE][MAX_LAYOUT_SIZE];		// layout drawing
	int access[MAX_LAYOUT_SIZE][MAX_LAYOUT_SIZE];		// defines accessibility for robots for shotest path solver. Allocated in WarehouseLayout/LayoutUI.cpp

	std::array<std::array<int, MAX_LAYOUT_SIZE>, MAX_LAYOUT_SIZE> acc;

	WarehouseLocation WL[MAX_LAYOUT_SIZE][MAX_LAYOUT_SIZE];		// for defining location its type for each [row][column]
																// Preallocated memory even though won't use all
};


struct TruckInfo {
	int ntrucks;			// number trucks
	int tloc[MAX_BOTS][2];  // truck locations [idx][col:0 row:1]
	bool re_docked;		
	bool del_docked;
};




struct RunnerInfo {
	int nrunners;			// number runners
	int rloc[MAX_BOTS][2];  // runner locations [idx][col:0 row:1]
};

struct RRbot : RunnerInfo {
	int ID;
	int Maxcapacity;	// max capacity in kg
	int Curcapacity;	// current capacity in kg
	//bool occupied;
	bool working;
	int path[MAX_LAYOUT_SIZE][MAX_LAYOUT_SIZE];
	bool drawpath;
};


struct SharedData {
	LayoutInfo layinfo;    
	RunnerInfo rinfo;
	RRbot rrbot_info[MAX_BOTS];  // Retieve and restock bot info
	TruckInfo tinfo;

	bool quit;         // tell everyone to quit
	int  magic;        // magic number for detecting layout initialization
	int magicW;			// magic number for detecting warehouse initialization

};






#endif	//WAREHOUSE_COMMON_H

//struct Point
//{
//	Point() : x(), y() {}
//	Point(int x, int y) : x(x), y(y) {}
//	int x, y;
//};
//
//bool operator<(const Point & lhs, const Point & rhs) // lhs = left-hand side
//													 // rhs = right-hand side
//{
//	if (lhs.x != rhs.x)
//	{
//		return lhs.x < rhs.x;
//	}
//	else
//	{
//		return lhs.y < rhs.y;
//	}
//}

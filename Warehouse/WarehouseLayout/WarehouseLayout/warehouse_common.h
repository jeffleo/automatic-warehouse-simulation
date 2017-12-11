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

#include <array>
#include <vector>

#include <cpen333/process/mutex.h>
#include <cpen333/process/shared_memory.h>
#include <cpen333/process/subprocess.h>

struct WarehouseLocation {
	// int type;
	int row;
	int col;

	// allocated while stocking
	int shelflevel;					// for storing item location vertically, IF -1 this is a truck location
	bool occupied;					// indication for empty/not empty shelf

	int leftrightmiddle;			// -1 for left, 1 for right, 0 for middle (aka not shelf)		
									// when storing items, decide whether or not left or right shelf

	WarehouseLocation() : occupied(false), leftrightmiddle(0), row(), col(), shelflevel(0) { }
	WarehouseLocation(int row, int col) : occupied(false), leftrightmiddle(0), shelflevel(0), row(row), col(col) {}		// for locating basic points
	WarehouseLocation(int row, int col, int leftrightmiddle, int shelflevel) : 
		occupied(true), leftrightmiddle(leftrightmiddle), row(row), col(col), shelflevel(shelflevel) {}		// for locating item stocking; NOTE DEFAULT occupied = true
};

struct LayoutInfo {
	int rows;           // rows in layout
	int cols;           // columns in layout
	char layout[MAX_LAYOUT_SIZE][MAX_LAYOUT_SIZE];		// layout drawing
	int access[MAX_LAYOUT_SIZE][MAX_LAYOUT_SIZE];		// defines accessibility for robots for shotest path solver. Allocated in WarehouseLayout/LayoutUI.cpp

	std::array<std::array<int, MAX_LAYOUT_SIZE>, MAX_LAYOUT_SIZE> acc;

	/*int visits[MAX_LAYOUT_SIZE][MAX_LAYOUT_SIZE];*/


	WarehouseLocation WL[MAX_LAYOUT_SIZE][MAX_LAYOUT_SIZE];		// for defining location its type for each [row][column]
																// Preallocated memory even though won't use all
	int shelves_rows;		// rows of shelfs 
	int shelves_cols;		// cols of shelfs 

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
	bool quit;         // tell everyone to quit
	int  magic;        // magic number for detecting layout initialization
	int magicW;			// magic number for detecting warehouse initialization
	
};

enum Tasktype {
	TTneither, TTdelivery, TTrestock
};

struct Task {
	unsigned long ID;
	int type;							// 1 for delivery, 2 for restock, ... 0 for neither
	std::vector<WarehouseLocation> path;

	Task(unsigned long ID, enum Tasktype tasktype) : ID(ID), type(tasktype) {}
	Task() {}
};


std::map<int, int> Task2CustMap;			// todo map task id to customer id and order id



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

#ifndef LAYOUT_COMMON_H
#define LAYOUT_COMMON_H

#define LAYOUT_MEMORY_NAME "layout_memory"
#define LAYOUT_MUTEX_NAME "layout_mutex"

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

#define MAX_LAYOUT_SIZE 80
#define MAX_BOTS 50

struct LayoutInfo {
	int rows;           // rows in layout
	int cols;           // columns in layout
	int shelves;		
	char layout[MAX_LAYOUT_SIZE][MAX_LAYOUT_SIZE];  // layout storage
	int visits[MAX_LAYOUT_SIZE][MAX_LAYOUT_SIZE];

};

struct RunnerInfo {
	int nrunners;      // number runners
	int rloc[MAX_BOTS][2];   // runner locations [col][row]
};

struct RRbot : RunnerInfo {
	int capacity;
	bool occupied;
	bool working;
};

struct SharedData {
	LayoutInfo layinfo;    
	RunnerInfo rinfo;
	RRbot rrbot_info;  // Retieve and restock bot info
	bool quit;         // tell everyone to quit
	int  magic;        // magic number for detecting initialization
};

#endif	//LAYOUT_COMMON_H
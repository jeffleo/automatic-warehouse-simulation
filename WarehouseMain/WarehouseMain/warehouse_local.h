#pragma once
#ifndef WAREHOUSE_LOCAL_H
#define WAREHOUSE_LOCAL_H


#include <map>
#include <cpen333/thread/fifo.h>

static const char SHELF = 186;	// ║
static const char DDOCK = 194;	// ┬
static const char RDOCK = 193;	// ┴
const int nrobots = 3;

std::map<std::pair<int, int>, WarehouseLocation&> ShelfMap;

WarehouseLocation del_truck;			// delivery location
WarehouseLocation res_truck;			// restock location
WarehouseLocation park(1, 1);			// parking/waiting/starting locations for robots

cpen333::thread::fifo<Task> tasks_Q(MAX_TASKS);
std::map<std::string, int> ItemMap;

//////////// FOR PREALLOCATING MEMORY
std::vector<SharedData*> shares;

int memoryIndex;
#endif //WAREHOUSE_LOCAL_H
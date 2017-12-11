#pragma once
#ifndef WAREHOUSE_LOCAL_H
#define WAREHOUSE_LOCAL_H


#include <map>
#include <cpen333/thread/fifo.h>

static const char SHELF = 186;	// ║
static const char DDOCK = 194;	// ┬
static const char RDOCK = 193;	// ┴
const int nrobots = 3;

// ITEM RELATED DATA
std::map<std::pair<int, int>, WarehouseLocation&> ShelfMap;
struct ItemInfo {
	std::vector<std::pair<bool, WarehouseLocation>> locations;
	int quantity;
	int mass;
};

//TODO GENERATE MASS

WarehouseLocation del_truck;			// delivery location
WarehouseLocation res_truck;			// restock location
WarehouseLocation park(1, 1);			// parking/waiting/starting locations for robots

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

cpen333::thread::fifo<Task> tasks_Q(MAX_TASKS);


std::map<int, int> Task2CustMap;			// todo map task id to customer id and order id

//std::map<std::string, int> ItemMap;

std::map<int, WarehouseLocation> ItemMap;


//////////// FOR PREALLOCATING MEMORY
std::vector<SharedData*> shares;


int memoryIndex;
#endif //WAREHOUSE_LOCAL_H
#pragma once
#ifndef WAREHOUSE_LOCAL_H
#define WAREHOUSE_LOCAL_H



#include <deque>
#include <map>
#include <vector>
#include <cpen333/thread/fifo.h>
#include <cpen333\thread\semaphore.h>
#include <cpen333\thread\condition.h>
#include <random>


const int nrobots = 5;

////	ORDER 
//list of orders received, orders ready for delivery, and orders out for delivery, ALL QUEUES

//struct OrderInfo {
//	// id and long vector of itemids
//	int customerID;
//	std::map<int, int> itemmap;
//	std::vector<int> itemvect;
//};

//// SHELF MAP FOR LOCATING FOR RESTOCKING
std::map<std::pair<int, int>, WarehouseLocation&> ShelfMap;			// KEY row , col
int shelves_rows;		// rows of shelfs 
int shelves_cols;		// cols of shelfs 

//// STOCK / ITEM -> MAPPED TO A location
int _currentstockquant = 0;

//std::map<std::string, int> ItemMap;
std::map<int, WarehouseLocation> ItemMap;		// maps id to a location

struct ItemInfo {
	std::vector<std::pair<bool, WarehouseLocation>> locations;
	int quantity;
	float mass;

	ItemInfo(int quantity, float mass) : quantity(quantity), mass(mass) {}
	ItemInfo() : quantity(0), mass(0) {}
};



//TODO GENERATE MASS

//// SPECIAL LOCATIONS
static const char SHELF = 186;	// ║
static const char DDOCK = 194;	// ┬
static const char RDOCK = 193;	// ┴

WarehouseLocation del_truck;			// delivery location
WarehouseLocation res_truck;			// restock location
WarehouseLocation park(1, 1);			// parking/waiting/starting locations for robots

//// TASKS
enum Tasktype {
	TTneither, TTdelivery, TTrestock
};

struct Task {
	unsigned long ID;
	int type;							// 1 for delivery, 2 for restock, ... 0 for neither
	std::vector<WarehouseLocation> path;
	std::vector<int> items;				// a vector of item IDs

	Task(unsigned long ID, enum Tasktype tasktype) : ID(ID), type(tasktype) {}
	Task(unsigned long ID, enum Tasktype tasktype, std::vector<int> items) : ID(ID), type(tasktype), items(items){}
	Task() {}
};

cpen333::thread::fifo<Task> tasks_Q(MAX_TASKS);

std::map<int, int> Task2CustMap;								// TODO map customer id TO task id as tasks are generated
/////////////TRUCK

struct TruckData{
	unsigned long ID;
	//std::array<int, MAXWAREHOUSESTOCK> items;	// a single array of items
	std::vector<int> itemsvect;						// a single array of items
	std::map<int, int> itemsmap;				// a map of items_id and its quantity

	int type;							// 1 for delivery, 2 for restock, ... 0 for neither
	unsigned int capacity;						// current item capacity in kg

	TruckData(unsigned long ID, enum Tasktype tasktype, std::vector<int> itemsvect, unsigned int capacity) : ID(ID), type(tasktype), itemsvect(itemsvect), capacity(capacity) {}
	TruckData(unsigned long ID, enum Tasktype tasktype, std::map<int, int> itemsmap, unsigned int capacity) : ID(ID), type(tasktype), itemsmap(itemsmap), capacity(capacity) {}

	TruckData() {}
};


// for trucks to race to aquiring a spot at the dock
cpen333::thread::semaphore deliverydock(0);
cpen333::thread::semaphore restockdock(0);

// for truck to notify dock handler thread that truck has docked
cpen333::thread::semaphore delivery_arrived(0);
cpen333::thread::semaphore restock_arrived(0);

// for notifying warehouse/truck is empty
cpen333::thread::semaphore deliveryempty(0);
cpen333::thread::semaphore restockempty(0);
cpen333::thread::condition cv_res;
cpen333::thread::condition cv_del;
unsigned long restocktaskid = 0;				// counter
unsigned long deliverytaskid = 0;				// counter

// truck loading buffers // when truck docks, it will load the queue and notify handler
std::mutex mutex_rbuff;
std::deque<int> res_buff;
std::mutex mutex_dbuff;
std::deque<int> del_buff;

//////////// FOR PREALLOCATING MEMORY
std::vector<SharedData*> shares;
int memoryIndex;


//// helpful functions
// limited to generate random integers between -1 - inf
int randomnum(float min, float max) {
	// single instance of random engine and distribution
	//static std::default_random_engine rnd;
	std::default_random_engine rnd(
		std::chrono::system_clock::now().time_since_epoch().count());	// seed rand function with time to generate different random value each time

	std::uniform_real_distribution<double> dist(min, max);

	int num = (int)ceil(dist(rnd));
	if (num > 0) return num;
	else return -1;
	
}
#endif //WAREHOUSE_LOCAL_H
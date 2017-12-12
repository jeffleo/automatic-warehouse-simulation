#include "..\..\Warehouse\WarehouseLayout\WarehouseLayout\warehouse_common.h"

#include <string>
#include <fstream>
#include <thread>

#include "ProductLib.h"
#include "ProductLibApi.h"
#include "JsonProductConverter.h"
#include "JsonProductLibraryApi.h"
#include "WarehouseComm.h"

#include <cpen333/process/mutex.h>
#include <cpen333/process/shared_memory.h>
#include <cpen333/process/subprocess.h>
#include <cpen333/process/socket.h>
#include <stdlib.h>

#include "truck.h"
#include "robots.h"
#include "warehouse_local.h"
#include "ShortestPath.h"
#include "stock.h"

#include <map>
#include <string>

#include <math.h>
#include <vector>


bool tryleft(int temp[MAX_LAYOUT_SIZE][MAX_LAYOUT_SIZE], int &c, int &r, int& target) {
	if (c > 0 &&  temp[c - 1][r] == target) { // try going left
		target--;
		--c;
		return 1;
	}
	else return 0;
}

bool tryright(int temp[MAX_LAYOUT_SIZE][MAX_LAYOUT_SIZE], int &c, int &r, int& target) {
	if (c < MAX_LAYOUT_SIZE && temp[c + 1][r] == target) { // try going right
		target--;
		++c;
		return 1;
	}
	else return 0;
}

bool tryup(int temp[MAX_LAYOUT_SIZE][MAX_LAYOUT_SIZE], int &c, int &r, int& target) {
	if (r > 0 &&  temp[c][r - 1] == target) { // try going up
		target--;
		--r;
		return 1;
	}
	else return 0;
}

bool trydown(int temp[MAX_LAYOUT_SIZE][MAX_LAYOUT_SIZE], int &c, int &r, int& target) {
	if (r <MAX_LAYOUT_SIZE && temp[c][r + 1] == target) { // try going down
		target--;
		++r;
		return 1;
	}
	else return 0;
}

bool movef(int temp[MAX_LAYOUT_SIZE][MAX_LAYOUT_SIZE], int dir, int &c, int &r, int &target) {
	dir = (dir % 4 + 4) % 4;// proper modulus operator in C++

	//std::cout << "trying movef with dir " << dir << " c " << c << " r " << r << std::endl;
	switch (dir) {
	case 0:
		if (!tryright(temp, c, r, target)) return 0;
		break;
	case 1:
		if (!trydown(temp, c, r, target)) return 0;

		break;
	case 2:
		if (!tryleft(temp, c, r, target)) return 0;
		break;
	case 3:
		if (!tryup(temp, c, r, target)) return 0;
		break;
	}
	return 1;
}


void PathGenRepeater(MazeSolver& shortest, std::vector<WarehouseLocation>& path, WarehouseLocation& startloc, WarehouseLocation& endloc) {
	// Start to waypoint
	int temp[MAX_LAYOUT_SIZE][MAX_LAYOUT_SIZE];
	shortest.iPath = 10;							// END MARKER

	startloc.col += startloc.leftrightmiddle;		// Adjusts position if given shelf location
	endloc.col += endloc.leftrightmiddle;

	if (shortest.FindPath(startloc.row, startloc.col, endloc.row, endloc.col)) {
		// path found

		for (int i = 0; i < MAX_LAYOUT_SIZE; ++i) {
			for (int j = 0; j < MAX_LAYOUT_SIZE; ++j) {
				temp[i][j] = shortest.iMazeSolved[i][j];	// copy path over
															//std::lock_guard<decltype(mutex_)> lock(mutex_);
															//shared->pathtest[i][j]= shortest.iMazeSolved[i][j];
			}
		}

		//std::lock_guard<decltype(mutex_)> lock(mutex_);
		//shared->drawpath = true;

		path.push_back(startloc);
		path.back().leftrightmiddle = 0;										// For making a clean start point (no stock)
		path.back().shelflevel = 0;												// For making a clean start point (no stock)
		WarehouseLocation temppoint;

		int r = startloc.col; int c = startloc.row; int dir = 0;
		int target = temp[startloc.row][startloc.col] -1;				// next to find
		
		// finds path from start to end in temp
		while (target != 10) {			// 10 marks end of path in shortestpath generator
			dir = (dir % 4 + 4) % 4;	// proper modulus operator in C++
			if (!movef(temp, dir, c, r, target)) dir++;				// keep turning until found direction
			else {
				// successful moved along path, save it
				temppoint.col = r;
				temppoint.row = c;
				path.push_back(temppoint);
			}
		}
		path.push_back(endloc);												// Pushes all details about stock location, shelf, lefy/right ect.

		// TEST DRAW PATH MARKED WITH 254
		//for (int r = 0; r < MAX_LAYOUT_SIZE; ++r) {
		//	for (int c = 0; c < MAX_LAYOUT_SIZE; ++c) {
		//		int ch = temp[r][c];
		//		if (ch == 254) {
		//			display_.set_cursor_position(YOFF + r, XOFF + c);
		//			std::printf("%c", 254);		// block
		//		}
		//	}
		//}

	}
	else {
		std::cout << "Path not solvable" << std::endl;							// will skip from start to end
		path.push_back(startloc);
		path.back().leftrightmiddle = 0;										// For making a clean start point (no stock)
		path.back().shelflevel = 0;												// For making a clean start point (no stock)
		path.push_back(endloc);												// Pushes all details about stock location, shelf, lefy/right ect.
	}
}

std::vector<WarehouseLocation> PathGenerator(MazeSolver& shortest, WarehouseLocation& startloc, std::vector<WarehouseLocation>& endlocs) {
	std::vector<WarehouseLocation> path;

	PathGenRepeater(shortest, path, startloc, endlocs.at(0));
	for (size_t i = 1; i< endlocs.size(); ++i) {
		//std::cout << " Generating path "<< i << std::endl;
		PathGenRepeater(shortest, path, endlocs.at(i-1), endlocs.at(i));
	}


	return path;
}

void GenerateDeliveryTask(unsigned long &taskid, MazeSolver& shortest, Task &DeliveryTask) {

	////// TESTING :: GO TO EACH SHELF COLUMN FROM SHELF 2, THEN GO TO DELIVERY TRUCK, THEN PARK
	//std::vector<WarehouseLocation> endlocs1;
	//for (int i = 0; i < 3; ++i) {
	//	std::pair<int, int> key = std::make_pair(2, i);
	//	auto it = ShelfMap.find(key);
	//	if (it != ShelfMap.end()) {
	//		//found 
	//		it->second.leftrightmiddle = 1;				//GO TO RHS OF SHELF
	//		endlocs1.push_back(it->second);
	//	}
	//}

	//for (int i = 0; i < 3; ++i) {
	//	std::pair<int, int> key = std::make_pair(7, i);
	//	auto it = ShelfMap.find(key);
	//	if (it != ShelfMap.end()) {
	//		//found 
	//		it->second.leftrightmiddle = -1;		//GO TO LHS OF SHELF
	//		endlocs1.push_back(it->second);
	//	}
	//}

	//for (int i = 4; i < 7; ++i) {
	//	std::pair<int, int> key = std::make_pair(12, i);
	//	auto it = ShelfMap.find(key);
	//	if (it != ShelfMap.end()) {
	//		//found 
	//		it->second.leftrightmiddle = 1;		//GO TO RHS OF SHELF
	//		endlocs1.push_back(it->second);
	//	}
	//}
	//endlocs1.push_back(del_truck);
	//endlocs1.push_back(park);

	//DeliveryTask.path = PathGenerator(shortest, park, endlocs1);
	//taskid++;

	std::vector<WarehouseLocation> endlocs1;
	

	for (int i = 0; i < DeliveryTask.items.size(); ++i) {

		auto it = StockInfo_map.find(DeliveryTask.items.at(i));
		if (it != StockInfo_map.end()) {
			//found 

			endlocs1.push_back(it->second.locations.back().second);			
			it->second.locations.pop_back();							// delete last location since it will be taken out for delivery

		}
		else {
			std::cout << "GenerateDeliveryTask: Don't have this item in stock: "<< DeliveryTask.items.back()  << std::endl;			// FOR DEBUGGING // try again

		}
		
	}

	endlocs1.push_back(del_truck);
	endlocs1.push_back(park);

	DeliveryTask.path = PathGenerator(shortest, park, endlocs1);
	taskid++;
	cv_del.notify_one();
}

void GenerateRestockTask(unsigned long &taskid, MazeSolver& shortest, Task &RestockTask) {
	//// PATH GENERATION: GO TO RESTOCK TRUCK, THEN GO TO RANDOM SHELF COL AND ROW, THEN PARK, 
	std::vector<WarehouseLocation> endlocs2;
	endlocs2.push_back(res_truck);

	for (int i = 0; i < RestockTask.items.size(); ++i) {
		bool emptylocation = false;

		while (!emptylocation) {
			std::pair<int, int> key = std::make_pair(randomnum(0, shelves_rows), randomnum(0, shelves_cols));		//// rows / cols -> MAPPED TO A LOCATION 
			auto it = ShelfMap.find(key);
			if (it != ShelfMap.end()) {
				//found 

				// CHECKING OCCUPATION

				// int leftright = randomnum(0, 2);		//GO TO LEFT OR RIGHT OF SHELF RANDOMLY
				// int shelf = randomnum(0, 6);
				//if (it->second.occupied[leftright][shelf] == false) {
				//	//it->second.leftrightmiddle = ;
				//	endlocs2.push_back(it->second);
				//	emptylocation = true;
				//}
				//else {

				//}
				it->second.leftrightmiddle = randomnum(-1.0,1.0);
				endlocs2.push_back(it->second);							// just any random shelf location will do for now
				emptylocation = true;
			}
			else {
				//std::cout << " Wrong shelf input" << std::endl;			// FOR DEBUGGING // try again
				
			}
		}

	}

	endlocs2.push_back(park);
	RestockTask.path = PathGenerator(shortest, park, endlocs2);
	taskid++;
	cv_res.notify_one();

	////// TEST PATH GENERATION: GO TO RESTOCK TRUCK, THEN GO TO EACH SHELF COLUMN FROM SHELF 2, THEN PARK
	//std::vector<WarehouseLocation> endlocs2;
	//endlocs2.push_back(res_truck);

	//for (int i = 0; i < 6; ++i) {
	//	std::pair<int, int> key = std::make_pair(2, i);
	//	auto it = ShelfMap.find(key);
	//	if (it != ShelfMap.end()) {
	//		//found 
	//		it->second.leftrightmiddle = 1;		//GO TO RHS OF SHELF
	//		endlocs2.push_back(it->second);
	//	}
	//}

	//endlocs2.push_back(park);
	//RestockTask.path = PathGenerator(shortest, park, endlocs2);

	//taskid++;
}

void map2vect(std::map<int, int>& items, std::vector<int> &itemvect, bool print) {

	for (std::map<int, int>::iterator it = items.begin(); it != items.end(); ++it) {
		if (print) std::cout << "Quantity:" << it->second << " => ItemID: " << it->first << '\n';

		for (int i = 0; i < it->second; ++i) {
			itemvect.push_back(it->first);
		}
		
	}
}

// CALL/CREATES RESTOCK TRUCK THREAD GIVEN INPUT ITEM_ID->QUANTY MAP
void CallRestock(std::map<int, int> &items, unsigned int &truckcounter, int &memoryindex, std::vector<truck*>& trucks ) {
	std::vector<int> itemvect;
	std::cout << "Requesting Restock of:\n" << std::endl;	
	map2vect(items, itemvect, true);

	std::vector<int> restockitems;
		
	if (_currentstockquant + itemvect.size() < MAXWAREHOUSESTOCK) {	// check if warehouse stock overstock first
		
		// count weight of items and partition truck calls
		float weight = 0;
		int i = 0;
		//for (std::map<int, int>::iterator it = items.begin(); it != items.end(); ++it) {	// iterate through entire map, accumulating weight
		for (int item : itemvect){

			auto itstock = StockInfo_map.find(item);
			if (itstock != StockInfo_map.end()) {
				//found 
				if (weight + itstock->second.mass >= MAXTRUCKCAPACITY) {
					std::cout << " Restock truck overloaded, sending out multiple restock truck for this restock. Count "<< i++ << std::endl;

					TruckData truckdata(truckcounter, TTrestock, restockitems, weight);
					//TruckData truckdata(truckcounter, TTrestock, items, weight);
					trucks.push_back(new truck(res_truck, memoryindex, truckdata));	// push restock truck thread
					trucks.back()->start();											// starts a restock truck thread	// VALIDATE
											

					truckcounter++;
					restockitems.clear();												// clear restock list for next truck
					weight = 0;														// reset for another truck
				}
				weight += itstock->second.mass;
				restockitems.emplace_back(item);
				//itemvect.pop_back();
			}
		}

		
		if (restockitems.size() > 0) {
			TruckData truckdata(truckcounter, TTrestock, restockitems, weight);
			trucks.push_back(new truck(res_truck, memoryindex, truckdata));		// push restock truck thread
			trucks.back()->start();												// starts a restock truck thread	// VALIDATE
			truckcounter++;

		}
	}
	else {
		std::cout << "WAREHOUSE STOCK OVERFLOW, RESTOCK ORDER CANCELED" << std::endl;
	}
}

// HANDLES RESTOCK TRUCK DOCKING SEMEPHORES, UNLOADING ITEMS, AND GENERATING RESTOCK TASKS FOR ROBOTS
void res_dockHandler(MazeSolver &shortest) {
	std::cout << "Restock dock handler thread started \n";
	

	while (true) {
		restockdock.notify();		// open dock 
		restock_arrived.wait();		// wait for a truck to arrive
		std::cout << "Restock truck arrival detected, handler thread creating restock tasks \n";

		// todo read restock truck buffer of items
		// GENERATE TASK FOR ROBOT TO PICKUP FROM TRUCK, partition by robot capacity
		std::vector<int> restockitems;

		float weight = 0;
		int i = 1;
		std::vector<Task> restocktasks;

		std::unique_lock<decltype(mutex_rbuff)> rlock(mutex_rbuff);
		while (!res_buff.empty()) {

			auto itstock = StockInfo_map.find(res_buff.back());
			if (itstock != StockInfo_map.end()) {
				//found 
				if (weight + itstock->second.mass >= MAX_ROBOT_CAPACITY) {
					std::cout << " Robot overloaded, sending out multiple robots for this restock. Count " << i++ << std::endl;

					restocktasks.push_back(Task(restocktaskid, TTrestock, restockitems));
					GenerateRestockTask(restocktaskid, shortest, restocktasks.back());

					restockitems.clear();													// reset vector for next robot
					float weight = 0;														// reset weight for next robot
				}
				weight += itstock->second.mass;
				restockitems.emplace_back(res_buff.back());
				res_buff.pop_back();
			}
			
		}
		

		if (restockitems.size() > 0) {
			restocktasks.push_back(Task(taskid, TTrestock, restockitems));
			GenerateRestockTask(taskid, shortest, restocktasks.back());

		}
		std::cout << "Done generating restock tasks. Created : "<< i << std::endl;

		for (Task task : restocktasks) {
			tasks_Q.push(task);													// push into q for robots
		}
		restocktasks.clear();
		
	}
}


// CALL/CREATES DELIVERY TRUCK THREAD GIVEN INPUT ITEM_ID->QUANTY MAP
void CallDelivery(struct OrderInfo order, unsigned int &truckcounter, int &memoryindex, std::vector<truck*>& trucks) {
		// TODO PASS ORDER INFO INTO TRUCK SO DELIVERY TRUCK KNOWS WHICH ORDER IT FULFILLED

	std::vector<int> itemvect = order.itemvect;
	std::vector<int> deliveryitems;
	std::cout << "Requesting Delivery of:\n" << std::endl;

	if (_currentstockquant - (int)(itemvect.size()) >= 0) {	// check if warehouse stock underflows first
		
		// count weight of items and partition truck calls
		float weight = 0;
		int i = 0;
		for (auto item : itemvect) {	// iterate through entire map, accumulating weight
			std::cout << " ItemID: " << item << '\n';

			auto itstock = StockInfo_map.find(item);
			if (itstock != StockInfo_map.end()) {													// ITEM FOUND IN INVENTORY
				if (itstock->second.quantity > 0) {												// ITEM IN STOCK
					itstock->second.quantity--;														// DECREASE ITEM COUNT
					_currentstockquant--;

					//found 
					if (weight + itstock->second.mass >= MAXTRUCKCAPACITY) {
						std::cout << " Delivery truck overloaded, sending out multiple delivery truck for this delivery. Count " << i++ << std::endl;

						TruckData truckdata(truckcounter, TTrestock, deliveryitems, weight);
						//TruckData truckdata(truckcounter, TTrestock, items, weight);
						trucks.push_back(new truck(res_truck, memoryindex, truckdata));	// push restock truck thread
						trucks.back()->start();											// starts a restock truck thread	// VALIDATE

						truckcounter++;
						deliveryitems.clear();												// clear restock list for next truck
						weight = 0;														// reset for another truck
					}
					weight += itstock->second.mass;
					deliveryitems.emplace_back(item);
					//itemvect.pop_back();
				}
				else {
					std::cout << " Item out of stock. Canceling order for customer: " << order.customerID << std::endl;
				}
			}
			else {
				std::cout << " Don't have this Item. Canceling order for customer: "<< order.customerID << std::endl;
			}
		}

		if (deliveryitems.size() > 0) {
			TruckData truckdata(truckcounter, TTdelivery, deliveryitems, weight);
			trucks.push_back(new truck(res_truck, memoryindex, truckdata));		// push delivery truck thread
			trucks.back()->start();												// starts a delivery truck thread	// VALIDATE
			truckcounter++;

			std::cout << "Order for customer: " << order.customerID << " called out for delivery" << std::endl;

		}
	}
	else {
		std::cout << "WAREHOUSE STOCK UNDERFLOW, DELIVERY ORDER CANCELED" << std::endl;
	}
}

// HANDLES DELIVERY TRUCK DOCKING SEMEPHORES, UNLOADING ITEMS, AND GENERATING DELVIERY TASKS FOR ROBOTS
void del_dockHandler(MazeSolver &shortest) {
	std::cout << "Delivery dock handler thread started \n";

	while (true) {
		deliverydock.notify();		// open dock 
		delivery_arrived.wait();	// wait for a truck to arrive
		std::cout << "Delivery truck arrival detected, handler thread creating deliverydock tasks \n";

		// read delivery truck buffer of items
		// GENERATE TASK FOR ROBOT TO PICKUP FROM TRUCK, partition by robot capacity
		std::vector<int> deliveryitems;

		float weight = 0;
		int i = 1;
		std::vector<Task> deliverytasks;

		std::unique_lock<decltype(mutex_dbuff)> dlock(mutex_dbuff);
		while (!del_buff.empty()) {

			auto itstock = StockInfo_map.find(del_buff.back());
			if (itstock != StockInfo_map.end()) {
				//found 
				if (weight + itstock->second.mass >= MAX_ROBOT_CAPACITY) {
					std::cout << " Robot overloaded, sending out multiple robots for this delivery. Count " << i++ << std::endl;

					//deliveryitems.emplace_back(del_buff.front());							// for fixing missing last item bug
					deliverytasks.push_back(Task(deliverytaskid, TTdelivery, deliveryitems));
					GenerateDeliveryTask(deliverytaskid, shortest, deliverytasks.back());
					deliverytasks.clear();													// reset vector for next robot
					float weight = 0;														// reset weight for next robot
				}
				weight += itstock->second.mass;
				deliveryitems.emplace_back(del_buff.back());
				del_buff.pop_back();
			}
			else {
				std::cout << " del_dockHandler : item not found" << std::endl;
			}

		}

		if (deliveryitems.size() > 0) {
			deliverytasks.push_back(Task(taskid, TTdelivery, deliveryitems));
			GenerateDeliveryTask(taskid, shortest, deliverytasks.back());

		}
		std::cout << "Done generating delivery tasks. Created : " << i << std::endl;

		for (Task task : deliverytasks) {
			tasks_Q.push(task);													// push into q for robots
		}
		deliverytasks.clear();

	}
}

void init_client_socket(std::string& server, int& port) {

	std::cout << "Enter address: ";
	std::cin >> server;


	std::cout << "Trying default port 5120 " << std::endl;
	port = 5120;

	//std::cout << "Enter port:    ";
	//std::cin >> port;
	//std::cout << std::endl;

	if (port == 0)	port = CPEN333_SOCKET_DEFAULT_PORT;

	// ignore the newline after we read a port, otherwise will
	//  interfere with reading a name
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	// create client and connect to socket
	std::cout << "connecting to port: " << port << std::endl;

}

void ClientOrderHandler(unsigned int &truckIDcounter, int &memoryIndex , std::vector<truck*> &trucks) {
	while (true) {

		std::unique_lock<std::mutex> uniquemutexref_(mutex_orderbuff);
		cv_server_order.wait(uniquemutexref_, [&]() { return orderbuff.size() == ORDERGROUPSIZE; });		// TO CHANGE??

		while (!orderbuff.empty()) {
			OrderInfo order = orderbuff.front();
			orderbuff.pop_front();
			CallDelivery(order, truckIDcounter, memoryIndex, trucks);
		}
	}
}

int main(int argc, char* argv[]) {
	//std::cout << "my directory is " << ExePath() << "\n";


	//if (uses) {
	//		std::string address;
	//		int port;
	//		init_client_socket(address, port);
	//
	//	//Initialize Socket
	//	cpen333::process::socket socket(address, WAREHOUSESERVERPORT);
	//	std::cout << "Connecting to Amazoom...";
	//	std::cout.flush();


	//	//GetOrdersFromServer(socket); //starts listening for orders coming in and writing it by reference to orderqueue;
	//	std::thread OrderThread(GetOrdersFromServer, std::ref(socket));
	//	OrderThread.detach();
	//}

	//std::vector<std::string> args3;
	//args3.push_back("cd ..\..\..\Warehouse/WarehouseLayout/x64/Debug/WarehouseLayout.exe");	// program name in arg[0]	// use ./ if up a folder
	//args3.push_back("././Warehouse/WarehouseLayout/x64/Debug/WarehouseLayout.exe");	// program name in arg[0]	// use ./ if up a folder
	//std::string filename = "data/layout3.txt";	
	//args3.push_back(filename); // second arg 
	//cpen333::process::subprocess layout(args3, true, true);		//detached, start now
	//system("cd ..\\..\\.\\Warehouse\\WarehouseLayout\\x64\\Debug && WarehouseLayout.exe data/layout3.txt");		// non detached but works

	//// memory init
	//cpen333::process::mutex mutex_(WAREHOUSE_MUTEX_NAME);
	//cpen333::process::shared_object<SharedData> memory(WAREHOUSE_MEMORY_NAME);
	//SharedData* shared = (SharedData*)memory.get();;	// get pointer to shared memory

	//if (shared->magic != 1454604) {
	//	std::cout << " Layout not started" << std::endl;
	//	cpen333::pause();
	//	return 0;
	//}

	
	//std::string usesockets;
	//std::cout << "Use sockets? (y/n) ";
	//std::cin >> usesockets;
	//bool uses = false;
	//if (usesockets.compare("y") == 0)	uses = true;
	//else uses = false;

	//if (uses) {
		////////////////////////////////////////////////////////////////////////////SOCKETS CODE
		cpen333::process::socket socket("localhost", WAREHOUSESERVERPORT);
		std::cout << "Connecting to Amazoom...";
		std::cout.flush();
		std::thread(GetOrdersFromServer, std::ref(socket)).detach();

	//}

	//OrderInfo test;
	//test.customerID = 1;
	//RejectOrder(test, socket);
;//////////////////////////////////////////////////////////////////////////////////////////////////////// PREAQUIRE MEMORY NAMES 
	int i = 0;
	std::string memoryname = WAREHOUSE_MEMORY_NAME + std::to_string(i++);

	cpen333::process::shared_object<SharedData> memory(memoryname);
	shares.push_back((SharedData*)memory.get());
	memory.unlink();																	// unlink for future use of named resource, optional


	memoryname = WAREHOUSE_MEMORY_NAME + std::to_string(i++);
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

	int MAX_NUM_WAREHOUSES = i;
///////////////////// HANDOUT UNIQUE MEMORY NAME
	SharedData* shared = shares.at(0);	// get pointer to shared memory
													//int magic = shared->magic;		// UNPROTECTED as no one else will write to magic

	int magicW = 0;									// ADDITIONAL MAGIC FOR WAREHOUSE MAIN
	int magic = 0;
	bool unique = false;
	i = 0;
	while (i < MAX_NUM_WAREHOUSES && !unique) {
		std::cout << "Checking for available layout: " << i << std::endl;
		//std::cout << "Memory name: " << memoryname << std::endl;
		shared = shares.at(i);												// get pointer to shared memory
		magic = shared->magic;
		if (magic == 1454604 + i) {											// CHECK IF LAYOUT HAS BEEN INITIALIZED, NOTE ==
			magicW = shared->magicW;
			if (magicW != 1454604 + i) {	
				std::cout << "Warehouse " << i << " memory allocated " << std::endl;
				memoryIndex = i;
				unique = true;
				shared->magicW = 1454604 + i;														//not init yet so aquire this magic 
																									// UNPROTECTED as no one else will write to magic
			}														
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
		std::cout << "Unique magic aquired: " << shared->magicW << std::endl;
	}


	cpen333::process::mutex mutex_(WAREHOUSE_MUTEX_NAME + std::to_string(memoryIndex));

	cpen333::pause();
	//END OF UNIQUE MEMORY AQUISISTION
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// shortest path solver init
	MazeSolver shortest(shared->layinfo.rows, shared->layinfo.cols, memoryIndex);

//////////////////////////////////////////////////////////////////////////////////	find all truck, shelve locations, and waiting stalls 

	LayoutInfo& linfo = shared->layinfo;

	// for counting quantity of shelves
	int shelfcounter_r = 0;
	int shelfcounter_c = 0;

	// for setting index of shelves
	int shelfindex_r = 0;
	int shelfindex_c = 0;

	// for keeping track of previously iterated cols or rows
	int lastrow = 0;
	int lastcol = 0;

	for (int r = 0; r < linfo.rows; ++r) {

		for (int c = 0; c < linfo.cols; ++c) {
			char ch = linfo.layout[c][r];

			if (ch == SHELF_CHAR) {
				// maps and counts shelves to rows and columns according the layout
				if (r != lastrow) {
					if (r > shelfcounter_r) {
						//std::lock_guard<decltype(mutex_)> lock(mutex_);
						shelves_rows++;	// counts once every time on new row, only till end  of linfo.rows as it does not get reset
					}

					shelfcounter_r = r;			// if been at this row, prevent from counting again
					shelfindex_r++;				// increments everytime on new row
					lastrow = r;				// keeps track of whether or not we've incremented shelfindex yet 
				}

				// always different col as we cycle through cols
				if (c != lastcol) {

					mutex_.lock();
					
					mutex_.unlock();

					std::pair<int, int> key = std::make_pair(shelfindex_r, shelfindex_c++);
					linfo.WL[c][r].row = r;				// save location
					linfo.WL[c][r].col = c;				// save location

					ShelfMap.insert({ key, linfo.WL[c][r]});

					if (c > shelfcounter_c) {
						//std::lock_guard<decltype(mutex_)> lock(mutex_);
						shelves_cols++;		// counts once every time on new col, only till end  of linfo.cols as it does not get reset
					}

					shelfcounter_c = c;				// if been at this column, prevent from counting again
					lastcol = c;					// keeps track of whether or not we've incremented shelfindex yet 
				}

			}
			else if (ch == DELIVERYDOCK_CHAR) {
				linfo.WL[c][r].row = r-1;				// save location
				linfo.WL[c][r].col = c;				// save location
				linfo.WL[c][r].leftrightmiddle = 0;
				linfo.WL[c][r].shelflevel = -1;

				del_truck = linfo.WL[c][r];		// copy to local			// fixed backwardness
			}
			else if (ch == RESTOCKDOCK_CHAR) {
				linfo.WL[c][r].row = r-1;			// save location
				linfo.WL[c][r].col = c;				// save location
				linfo.WL[c][r].leftrightmiddle = 0;
				linfo.WL[c][r].shelflevel = -1;

				res_truck = linfo.WL[c][r];			// copy to local		// fixed backwardness
			}
			//linfo.leftrightmiddle = 0;
			
		}
		shelfindex_c = 0;		// restart column for next row
	}

//////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
	// ROBOT INIT // 
	std::vector<robot*> robots;

	int startloc[2];
	startloc[0] = 1;
	startloc[1] = 1;
	for (int i = 0; i<nrobots; ++i) {
		robots.push_back(new robot(startloc, memoryIndex));
	}
	// start 
	for (auto& robot : robots) {
		robot->start();
	}
/////////////////////////////////////////////////////////////////////////////////////////////////
	// STOCK INIT //

	
	GenerateStockInfo(StockInfo_map);				// preallocation of all 30 itemsinfo in a stockinfo map with 0 quantity

	// item_id , quantity
	std::map<int, int> Restock_map;
	//RandomStockGenerator(Restock_map, 10);			// generate map of itemid -> quantity = 1, with num of types of items



	// TRUCK HANDLER THREAD INIT //
	std::thread(res_dockHandler, std::ref(shortest)).detach();
	std::thread(del_dockHandler, std::ref(shortest)).detach();						//TODO DELIVERY

	// TRUCK VECTOR INIT //
	std::vector<truck*> trucks;
	unsigned int truckIDcounter;

	// INITIAL WAREHOUSE RESTOCK CALL //
	
	/*CallRestock(Restock_map, truckIDcounter, memoryIndex, trucks);*/

//////////////////////////////////////////////////////////////////////////////////////////////////

	// 
	std::thread(ClientOrderHandler, std::ref(truckIDcounter), std::ref(memoryIndex), std::ref(trucks)).detach();


/////////////////////////////////////////////////////////////////////////////////////////////////
	unsigned long taskid = 0;			// counter
	
	//endlocs.push_back(WarehouseLocation(6, 9, 1, 2));		// basic zig zag test

	//Task DeliveryTask(taskid, TTdelivery);
	//GenerateDeliveryTask(taskid, shortest, DeliveryTask);

	//Task RestockTask(taskid,TTrestock);
	//GenerateRestockTask(taskid, shortest, RestockTask);


	std::string key = "menu";
	while (key != "e") {
		


		if (key == "d") {
			//tasks_Q.push(DeliveryTask);

			std::cout << "Requesting delivering of : \n";
			OrderInfo deliverytest;

			deliverytest.customerID = 0;
			map2vect(Restock_map, deliverytest.itemvect, true);
			CallDelivery(deliverytest, truckIDcounter, memoryIndex, trucks);
		}
		if (key == "r") {
			//tasks_Q.push(RestockTask);
			//std::cout << "Requesting restock of of : \n";
			Restock_map.clear();
			RandomStockGenerator(Restock_map, 3);			// generate map of itemid -> quantity = 1, with num of types of items
			CallRestock(Restock_map, truckIDcounter, memoryIndex, trucks);
		}
		if (key == "robot+1") {
			robots.push_back(new robot(startloc, memoryIndex));
			robots.back()->start();
		}
		if (key == "robot-1") {
			if (robots.size() > 0) {
				Task poison(0, TTpoison);
				tasks_Q.push(poison);
				robots.back()->join();
				robots.pop_back();
			}
		}
		if (key == "robotspeed") {
			int speed = 0;
			std::cin >> speed;
			if (speed > 0) {
				for (auto& robot : robots) {
					robot->robotspeed = speed;
				}
			}
		}

		if (key == "query") {
			int item = 0;
			std::cin >> item;
			if (item > 0) {
				int quantity = StockQuery(StockInfo_map, item);
				std::cout << item << " has quantity: " << quantity << std::endl;
			}
		}

		if (key == "restock") {
			char more = 'y';
			Restock_map.clear();
			while (more == 'y') {
				int item = 0;
				std::cin >> item;
				int quantity = 0;
				std::cin >> quantity;

				if (item > 0 && quantity > 0) {

					Restock_map.insert(std::make_pair(item, quantity));
					std::cout << "You chose to restock item: " << item << " quantity: " << quantity << std::endl;
					std::cout << "more items? write y " << std::endl;
					std::cin >> more;

					if (more != 'y') {
						CallRestock(Restock_map, truckIDcounter, memoryIndex, trucks);
					}

				}
				else std::cout << "Invalid" << std::endl;
			}
		}

		if (key == "deliver") {
			OrderInfo deliverytest;
			deliverytest.customerID = 0;
			std::map<int, int> delivery_map;
			char more = 'y';

			while (more == 'y') {
				int item = 0;
				std::cin >> item;
				int quantity = 0;
				std::cin >> quantity;

				if (item > 0 && quantity > 0) {
					delivery_map.insert(std::make_pair(item, quantity));
					std::cout << "You chose to deliver item: " << item << " quantity: " << quantity << std::endl;
					std::cout << "more items? write y " << std::endl;
					std::cin >> more;

					if (more != 'y') {
						map2vect(delivery_map, deliverytest.itemvect, true);
						CallDelivery(deliverytest, truckIDcounter, memoryIndex, trucks);
					}

				}
				else std::cout << "Invalid" << std::endl;
			}
		}
		

		if (key == "menu") {
			std::cout << "//////////////////////////WAREHOUSE MENU OPTIONS//////////////////////////////////\n";
			std::cout << "Enter e to exit, d for random delivery, r for random restock \n";
			std::cout << "query item_id<int> for quantity, restock item_id<int> quantity<int>,  \n";
			std::cout << "robot+1 to add one more robot, robot-1 to remove a robot, robotspeed speed<int> \n";
			std::cout << "//////////////////////////END OF MENU OPTIONS/////////////////////////////////////\n";
		}

		std::cin >> key;
		std::cout << "You pressed " << key << std::endl;

	}

	//Task2CustMap

	cpen333::pause();



//////////////////////////////////////////////////////////////////////////////////////////////////////
	//cpen333::pause();
	//// Signal all robots to quit
	////==================================================
	//Task Ptask;
	//Ptask.type = POISON;
	//for (int i = 0; i<nrobots; ++i) tasks_Q.push(Ptask);

	//// wait for all robots to quit
	//for (auto& robot : robots) {
	//	robot->join();
	//}


	
	return 0;
}


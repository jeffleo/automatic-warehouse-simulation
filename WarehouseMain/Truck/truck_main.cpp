#include "..\..\Warehouse\WarehouseLayout\WarehouseLayout\warehouse_common.h"

#include <cpen333/process/mutex.h>
#include <cpen333/process/shared_memory.h>
#include <cpen333/process/subprocess.h>
#include <cpen333/console.h>

#include <stdlib.h>
#include <array>
#include <string>

class Truck {
	// display offset for better visibility
	static const int XOFF = 2;
	static const int YOFF = 1;

	cpen333::console display_;
	cpen333::process::mutex mutex_;
	cpen333::process::shared_object<SharedData> memory_;
	

	int loc_[2];   // location

	unsigned int MaxCapacity = MAXTRUCKCAPACITY;

public:
	unsigned int CurCapacity;						// TO VALIDATE IF CAN READ
	int idx_;

	Truck(WarehouseLocation dockloc, int memoryindex) :
		 idx_(0), loc_(), memory_(WAREHOUSE_MEMORY_NAME + std::to_string(memoryindex)), mutex_(WAREHOUSE_MUTEX_NAME + std::to_string(memoryindex)) {

		loc_[COL_IDX] = dockloc.col;
		loc_[ROW_IDX] = dockloc.row -1;


	}

	void Restock();
	void Delivery();

};

int main(int argc, char* argv[]) {


	return 0;
}
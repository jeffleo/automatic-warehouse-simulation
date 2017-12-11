//#include "..\..\Warehouse\WarehouseLayout\WarehouseLayout\warehouse_common.h"
////
//
////#include <cpen333/thread/fifo.h>
////
////#include <cpen333/process/mutex.h>
////#include <cpen333/process/shared_memory.h>
////#include <cpen333/process/subprocess.h>
////
////
////#include "robots.h"
//#include "localtypes.h"
////
////
//
//#include <vector>
//#include <string>
//#include <fstream>
//#include <map>
//#include <stdlib.h>
//
//
//std::vector<WarehouseLocation> PathGenerator() {
//	std::vector<WarehouseLocation> path;
//
//
//	// TEST GO TO EACH COLUMN
//	for (int i = 0; i < 7; ++i) {
//		std::pair<int, int> key = std::make_pair(1, i);
//		auto it = ShelfMap.find(key);
//		if (it != ShelfMap.end()) {
//			//found 
//			path.push_back(it->second);		// push warehouse location
//		}
//	}
//
//
//
//	// TEST GO TO DELIVERY 
//
//
//	// TEST GO TO RESTOCK 
//
//
//
//
//	return path;
//}

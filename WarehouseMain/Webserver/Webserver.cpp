#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <memory>
#include <mutex>

#include "DynamicOrderQueue.h"

#include "ProductLib.h"
#include "ProductLibApi.h"
#include "JsonProductConverter.h"
#include "JsonProductLibraryApi.h"

#include <cpen333/process/socket.h>
#include <cpen333/process/mutex.h>
#include <cpen333/process/subprocess.h>

template<typename Out>
void split(const std::string &s, char delim, Out result) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		*(result++) = item;
	}
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

void initializeItemID(std::map<std::string,int> &ItemMap) {
	ItemMap.insert(std::pair<std::string,int>("UFO 02 Detector", 1));
	ItemMap.insert(std::pair<std::string, int>("Relaxman Relaxation Capsule", 2));
	ItemMap.insert(std::pair<std::string, int>("Milk Carton Hat", 3));
	ItemMap.insert(std::pair<std::string, int>("Inflatable Toast", 4));
	ItemMap.insert(std::pair<std::string, int>("Five-Pound Fat Replica", 5));
	ItemMap.insert(std::pair<std::string, int>("Proporta Elephant Camouflage Kit", 6));
	ItemMap.insert(std::pair<std::string, int>("Laparoscopic Gastric Bypass", 7));
	ItemMap.insert(std::pair<std::string, int>("Wheelmate Laptop Steering Wheel Desk", 8));
	ItemMap.insert(std::pair<std::string, int>("Hutzler 571 Banana Slicer", 9));
	ItemMap.insert(std::pair<std::string, int>("Accoutrements Horse Head Mask", 10));
	ItemMap.insert(std::pair<std::string, int>("Wolf Urine Lure 32 oz", 11));
	ItemMap.insert(std::pair<std::string, int>("Uranium Ore", 12));
	ItemMap.insert(std::pair<std::string, int>("Tuscan Whole Milk, 1 Gallon", 13));
	ItemMap.insert(std::pair<std::string, int>("Three Wolf Moon Short Sleeve Tee", 14));
	ItemMap.insert(std::pair<std::string, int>("Steering Wheel Attachable Work Surface Tray", 15));
	ItemMap.insert(std::pair<std::string, int>("English Grammar For Dummies", 16));
	ItemMap.insert(std::pair<std::string, int>("Asfour Crystal Tut Anhk Amon Chair", 17));
	ItemMap.insert(std::pair<std::string, int>("English Grammar For Dummies", 18));
	ItemMap.insert(std::pair<std::string, int>("ThinkGeek canned Unicorn Meat", 19));
	ItemMap.insert(std::pair<std::string, int>("Accoutrements Yodelling Pickle", 20));
	ItemMap.insert(std::pair<std::string, int>("Poo-Pourri Before-You-Go Toilet Spray", 21));
	ItemMap.insert(std::pair<std::string, int>("Delicious PhD Darling Costume", 22));
	ItemMap.insert(std::pair<std::string, int>("GEP Grouting, Cleaning and Washing Sponge", 23));
	ItemMap.insert(std::pair<std::string, int>("The Kitty Pass Interior Pet Door", 24));
	ItemMap.insert(std::pair<std::string, int>("Portable Pizza Pouch", 25));
	ItemMap.insert(std::pair<std::string, int>("Creative Arts by Charles Leonard Wiggle Eyes", 26));
	ItemMap.insert(std::pair<std::string, int>("Glow In The Dark Toilet Roll", 27));
	ItemMap.insert(std::pair<std::string, int>("Spandex Bodysuits", 28));
	ItemMap.insert(std::pair<std::string, int>("Bacon Bandages", 29));
	ItemMap.insert(std::pair<std::string, int>("Dab Fidget Spinner T-Shirt Cool Dabbing Tee", 30));
}

std::map<int, bool> RejectedFlag;

void ClientOrderRejection(std::mutex &cmutex, int id, ProductLibAPI &&api) {
	cmutex.lock();
	RejectedFlag.erase(id); //clear entry if already exist
	RejectedFlag.insert(std::pair<int, bool>(id, false)); //initiate new with false flag
	cmutex.unlock();

	while (true) {
		cmutex.lock();
		if (RejectedFlag.at(id) == true) {
			Product message("Your Order is rejected due to low stock!", ""); //sends message back
			AddMessage msg(message);
			api.sendMessage(msg);
			RejectedFlag.at(id) = false; //reset flag
			std::cout << "Customer " << id << "'s order is denied!" << std::endl;
		}
		cmutex.unlock();
	}
}

void service(std::map<int,std::vector<std::string>> &CustomerOrders, std::mutex &cmutex, ProductLib &lib, ProductLibAPI &&api, int id, DynamicOrderQueue &order_queue) {
	cpen333::process::mutex mutex("Server_Mutex");
	std::unique_lock<cpen333::process::mutex> memLock(mutex);
	memLock.unlock();

	// TODO

	std::cout << "Client " << id << " connected" << std::endl;
	// receive message
	std::unique_ptr<Message> msg = api.recvMessage();
	mutex.lock();

	// Initialize ItemID map
	std::map<std::string, int> WarehouseItems;
	initializeItemID(WarehouseItems);
	std::map<int, int> ItemBasket;

	// continue while we don't have an error
	while (msg != nullptr) {

		// react and respond to message
		MessageType type = msg->type();
		switch (type) {
		
		//Modified to add product to a map of customer orders
		case MessageType::ADD: {
			// process "add" message
			// get reference to ADD!
			AddMessage &add = (AddMessage &)(*msg);
			std::cout << "Client " << id << " making order: " << add.product << std::endl;

			// add product to order
			bool success = false;
			Product newproduct = add.product;
			std::vector<std::string> orderstring = split(newproduct.product, ';');
			
			//To-Do - make safety check to see if order can be fulfilled

			for (auto i : orderstring) {
				mutex.lock();
				std::vector<std::string> orderinfo = split(i, '+'); //1-product name, 2-amt.
				ItemBasket.insert(std::pair<int,int>(WarehouseItems[orderinfo[0]], std::stoi(orderinfo[1])));
				mutex.unlock();
			}

			//Adds an order to the queue
			Order neworder;
			neworder.customer_id = id;

			//Loops through the basket and create an order
			for (auto it = ItemBasket.cbegin(); it != ItemBasket.cend(); ++it) { 
				//std::cout << "CustomerID " << id << ": " << it->first << " " << it->second << std::endl;
				for (int i = 0; i < it->second; i++) {
					neworder.item.push_back(it->first);
				}
			}

			order_queue.add(neworder);

			// send response
			if (success) {
				api.sendMessage(AddResponseMessage(add, MESSAGE_STATUS_OK));
			}
			else {
				api.sendMessage(AddResponseMessage(add, MESSAGE_STATUS_ERROR, "failed to add order"));
			}
			break;
		}

		case MessageType::REMOVE: {
			RemoveMessage &remove = (RemoveMessage &)(*msg);
			std::cout << "Client " << id << " is removing product: " << remove.product << std::endl;

			bool success = false;
			success = lib.remove(remove.product);
			// send response
			if (success) {
				api.sendMessage(RemoveResponseMessage(remove, MESSAGE_STATUS_OK));
			}
			else {
				api.sendMessage(RemoveResponseMessage(remove,
					MESSAGE_STATUS_ERROR,
					"Product not found"));
			}

			break;
		}

		case MessageType::SEARCH: {
			// process "search" message
			// get reference to SEARCH
			SearchMessage &search = (SearchMessage &)(*msg);

			std::cout << "Client " << id << " searching for: "
				<< search.product_regex << " - " << search.price_regex << std::endl;

			// search library
			std::vector<Product> results;
			results = lib.find(search.product_regex, search.price_regex);

			// send response
			api.sendMessage(SearchResponseMessage(search, results, MESSAGE_STATUS_OK));
			break;
		}

		case MessageType::GOODBYE: {
			// process "goodbye" message
			std::cout << "Client " << id << " closing" << std::endl;
			return;
		}

		default: {
			std::cout << "Client " << id << " sent invalid message" << std::endl;
		}
		}

		// receive next message
		mutex.unlock();
		msg = api.recvMessage();
		mutex.lock();
	}
}

void warehouseservice(std::map<int, std::vector<std::string>> &CustomerOrders, std::mutex &cmutex, ProductLib &lib, ProductLibAPI &&api, int id, DynamicOrderQueue &order_queue) {
	cpen333::process::mutex mutex("Warehouse_Mutex");
	std::unique_lock<cpen333::process::mutex> memLock(mutex);
	memLock.unlock();
	
	std::cout << "Warehouse " << id << " connected" << std::endl;

	while (true) {
		//Retrieve order from queue
		Order receivedorder;
		receivedorder = order_queue.get();

		std::string mergedstringorders;
		//Process the order vector
		for (auto i : receivedorder.item) {
			mergedstringorders.append(std::to_string(i));
			mergedstringorders.append(";");
		}
		Product ordertosend = { std::to_string(receivedorder.customer_id),mergedstringorders };
		AddMessage msg(ordertosend);
		api.sendMessage(msg);
	}
}

/**
* Load products from a JSON file and add them to the music library
* @param lib music library
* @param filename file to load
*/
void load_products(ProductLib &lib, const std::string& filename) {

	// parse from file stream
	std::ifstream fin(filename);
	if (fin.is_open()) {
		JSON j;
		fin >> j;
		std::vector<Product> products = JsonConverter::parseProducts(j);
		lib.add(products);
	}
	else {
		std::cerr << "Failed to open file: " << filename << std::endl;
	}

}

void ClientSocketThread(DynamicOrderQueue &order_queue) {
	// load  data
	std::vector<std::string> filenames = {
		"data/AmazonProducts.json"
	};

	//Write the products to a common library
	ProductLib lib;      
	for (const auto &filename : filenames) {
		load_products(lib, filename);
	}

	cpen333::process::socket_server server(PRODUCTSERVERPORT);
	server.open();
	std::cout << "Client server started on port " << server.port() << std::endl;

	cpen333::process::socket client;
	int clientCount = 1;

	std::map<int, std::vector<std::string>> CustomerOrders;
	while (server.accept(client)) {

		// create API handler
		JsonProductLibraryApi api(std::move(client));
		// service client-server communication
		std::mutex clientmutex;
		std::thread thread(service, std::ref(CustomerOrders), std::ref(clientmutex), std::ref(lib), std::move(api), clientCount, std::ref(order_queue)); //Create new thread for each client
		thread.detach(); //Detach client to let it run on its own

		clientCount++;
	}
	// close server
	server.close();
}

void WarehouseSocketThread(DynamicOrderQueue &order_queue) {
	// load  data
	std::vector<std::string> filenames = {
		"data/AmazonProducts.json"
	};

	ProductLib lib;       // main shared music library
						  // load music library files
	for (const auto &filename : filenames) {
		load_products(lib, filename);
	}

	cpen333::process::socket_server server(WAREHOUSESERVERPORT);
	server.open();
	std::cout << "Warehouse server started on port " << server.port() << std::endl;

	std::vector<std::string> address = server.address_lookup();
	std::cout << "List of addresses: " << std::endl;
	for (const auto& add : address) {
		std::cout << "  " << add << std::endl;
	}
	std::cout << std::endl;

	cpen333::process::socket client;
	int clientCount = 1;

	std::map<int, std::vector<std::string>> CustomerOrders;
	while (server.accept(client)) {
		// create API handler
		JsonProductLibraryApi api(std::move(client));
		// service client-server communication
		std::mutex clientmutex;
		std::thread thread(warehouseservice, std::ref(CustomerOrders), std::ref(clientmutex), std::ref(lib), std::move(api), clientCount, std::ref(order_queue)); //Create new thread for each client
		thread.detach(); //Detach client to let it run on its own

		// NOT SURE IF CAUSING RUNTIME ISSUES
		//std::thread rejection(ClientOrderRejection, std::ref(clientmutex), clientCount, std::move(api)); // pass mutex to rejection checking
		//rejection.detach();


		clientCount++;
	}

	// close server
	server.close();
}

int main() {
	DynamicOrderQueue order_queue;
	//cpen333::process::subprocess mikey("./Client", true, true);

	std::thread thread1(ClientSocketThread, std::ref(order_queue)); //Create new thread port for client
	std::thread thread2(WarehouseSocketThread, std::ref(order_queue)); //Create new thread port for warehouse
	thread1.join();
	thread2.join();

	return 0;
}

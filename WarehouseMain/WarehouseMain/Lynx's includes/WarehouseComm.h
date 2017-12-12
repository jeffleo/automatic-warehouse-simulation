#ifndef WAREHOUSECOMM
#define WAREHOUSECOMM

#include "warehouse_local.h"
#include "ProductLib.h"
#include "JsonProductLibraryApi.h"
#include "DynamicOrderQueue.h"

#include <cpen333/process/socket.h>
#include <cpen333/process/mutex.h>
#include <cpen333/process/subprocess.h>

#include <iostream>
#include <limits>
#include <ctype.h>
#include <string.h>
#include <cstdlib>
#include <deque>


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

//need to initailize socket before hand, use 
//cpen333::process::socket socket("localhost", WAREHOUSESERVERPORT);
//std::cout << "Connecting to Amazoom...";
//std::cout.flush();

void GetOrdersFromServer(cpen333::process::socket &socket) {
	// if we open the socket successfully, continue
	if (socket.open()) {
		std::cout << "connected." << std::endl;

		// create API handler
		JsonProductLibraryApi api(std::move(socket));
		std::vector<Product> CurrentList;
		std::vector<Product> Cart;
		std::map<Product, int> Order;

		//InitalizeMap
		for (const auto& product : CurrentList) {
			Order.insert(std::pair<Product, int>(product, 0));
		}

		cpen333::process::mutex mutex("Warehouse_Mutex");
		std::unique_lock<cpen333::process::mutex> memLock(mutex);
		memLock.unlock();

		// receive message

		std::vector<std::string> incomingitems;
		std::vector<int> OrderItemList;
		std::unique_ptr<Message> msg = api.recvMessage();
		mutex.lock();

		// continue while we don't have an error
		while (msg != nullptr) {

			// react and respond to message
			MessageType type = msg->type();
			switch (type) {
				case MessageType::ADD: {
					AddMessage &add = (AddMessage &)(*msg);
					std::cout << "New Order Received!" << std::endl;
					// add product to order
					Product newproduct = add.product; //newproduct -> Customer ID, Array of items
					std::cout << "product " << newproduct.product << ": price " << newproduct.price << std::endl;   //prints out received items (debugging use)

					incomingitems = split(newproduct.price, ';');
					for (auto i : incomingitems) {
						OrderItemList.push_back(std::stoi(i));
					} //outputs into a vector of ints

					//push it into order queue
					OrderInfo receivedorder;
					receivedorder.customerID = std::stoi(newproduct.product);
					receivedorder.itemvect = OrderItemList;

					mutex_orderbuff.lock();
					orderbuff.emplace_back(receivedorder);
					mutex_orderbuff.unlock();
					cv_server_order.notify_one();			// let warehouse main. ClientOrderHandler know an order has come in

					break;
				}
				default: {
					std::cout << "Received invalid message" << std::endl;
				}
			}
			// receive next message
			mutex.unlock();
			msg = api.recvMessage();
			mutex.lock();
		}
	}
	else {
		std::cout << "failed." << std::endl;
	}
}


//need to initialize socket before hand, use 
//cpen333::process::socket socket("localhost", REJECTIONFEEDBACK);
//std::cout.flush();
void RejectOrder(OrderInfo RejectedOrder, cpen333::process::socket &socket) {
	std::string returnstring;
	for (auto i : RejectedOrder.itemvect) {
		returnstring.append(std::to_string(i));
	}

	// create API handler
	JsonProductLibraryApi api(std::move(socket));
	Product RejectedProducts(std::to_string(RejectedOrder.customerID), returnstring);
	AddMessage msg(RejectedProducts);
	api.sendMessage(msg);
}

#endif
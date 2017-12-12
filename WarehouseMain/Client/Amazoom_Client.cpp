#include "ProductLib.h"
#include "JsonProductLibraryApi.h"

#include <cpen333/process/socket.h>
#include <cpen333/process/mutex.h>

#include <iostream>
#include <limits>
#include <ctype.h>
#include <string.h>
#include <cstdlib>

static const char CLIENT_SEARCH = '1';
static const char CLIENT_ADD = '2';
static const char CLIENT_REMOVE = '3';
static const char CLIENT_ORDER = '4';
static const char CLIENT_QUIT = '5';

bool is_number(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

// print menu options
void print_menu() {
	std::cout << "========================================================" << std::endl;
	std::cout << "=                  Welcome to Amazoom!                 =" << std::endl;
	std::cout << "========================================================" << std::endl;
	std::cout << " (1) Search Items!" << std::endl;
	std::cout << " (2) Add Product To Cart" << std::endl;
	std::cout << " (3) Clear Cart" << std::endl;
	std::cout << " (4) Make Order" << std::endl;
	std::cout << " (5) Quit" << std::endl;
	std::cout << "=========================================" << std::endl;
	std::cout << "Please Choose Selection" << std::endl;
	std::cout.flush();
}


// search for songs on server
std::vector<Product> UpdateItemList(ProductLibAPI &api) {
	std::string product_regex, price_regex;
	product_regex = "";
	price_regex = "";

	// send search message and wait for response
	SearchMessage msg(product_regex, price_regex);
	if (api.sendMessage(msg)) {
		// get response
		std::unique_ptr<Message> msgr = api.recvMessage();
		SearchResponseMessage& resp = (SearchResponseMessage&)(*msgr);

		if (resp.status == MESSAGE_STATUS_OK) {
		}
		else {
		}
		return resp.results;
	}
	std::vector<Product> product;
	return product;
	std::cout << std::endl;
}

// adds locally
void do_add(std::vector<Product> &CurrentList,std::vector<Product> &Cart) {
	std::string result = "-1";
	std::string count;
	while (true) {
		std::cout << "Please input the item number you would like to add to cart (-1 to exit)" << std::endl;
		std::cin >> result;

		if (is_number(result) && std::stoi(result) > 0 && std::stoi(result) < CurrentList.size()) {
			std::cout << "How many of " << CurrentList[std::stoi(result)] << " would you like?" << std::endl;
			std::cin >> count;

			if (is_number(count) && std::stoi(count) > 0) {
				for (int amount = std::stoi(count); amount > 0; amount--) {
					Cart.push_back(CurrentList[std::stoi(result)]);
				}
			}
		}
		std::cout << "Current Items in Cart: " << std::endl;
		for (const auto& product : Cart) {
			std::cout << "   " << product << std::endl;
		}
		if (result == ("-1")) {
			break;
		}
	}
}

// remove locally from cart
void do_remove(std::vector<Product> &CurrentList, std::vector<Product> &Cart) {
	Cart.clear();
	std::cout << "Cart Cleared!" << std::endl; 
}

// search for products on server
std::vector<Product> do_search(ProductLibAPI &api) {
	std::string product_regex, price_regex;
	// collect regular expressions for song search
	std::cout << std::endl << "Search for Products" << std::endl;
	std::cout << "   Product Name Expression: ";
	std::getline(std::cin, product_regex);
	std::cout << "   Price Expression:  ";
	std::getline(std::cin, price_regex);

	// send search message and wait for response
	SearchMessage msg(product_regex, price_regex);
	if (api.sendMessage(msg)) {
		// get response
		std::unique_ptr<Message> msgr = api.recvMessage();
		SearchResponseMessage& resp = (SearchResponseMessage&)(*msgr);

		if (resp.status == MESSAGE_STATUS_OK) {
			std::cout << std::endl << "   Results:" << std::endl;
			int count = 1;
			for (const auto& product : resp.results) {
				std::cout << "   " << count << ".   " << product << std::endl;
				count++;
			}
		}
		else {
			std::cout << std::endl << "   Search \"" << product_regex << " - "
				<< price_regex << "\" failed: " << resp.info << std::endl;
		}
		return resp.results;
	}
	std::vector<Product> product;
	return product;
	std::cout << std::endl;
}

// search for songs on server
bool makeorder(ProductLibAPI &api, std::vector<Product> &Cart, std::map<Product, int> &Order) {

	std::string customerID, ProductString;

	if (Cart.size() == 0) {
		std::cout << "Please add items to your cart first!" << std::endl;
		return false;
	}

	//Start throwing the Cart items into a big map of orders
	for (auto item: Cart) {
		std::map<Product,int>::iterator it = Order.find(item);
		if (it != Order.end()) {
			//item exist in order already, just add to it
			it->second++;
		}
		else {
			Order.insert(std::pair<Product,int>(item, 1)); //initialize it, shouldn't go to this path ever
		}
	}

	std::string middelim = "+";
	std::string end = ";";
	for (auto it = Order.cbegin(); it != Order.cend(); ++it)
	{
		if (it->second == 0) {
			continue; //skip over if no item is in the slot
		}
		ProductString.append(it->first.product);
		ProductString.append(middelim);
		ProductString.append(std::to_string(it->second));
		ProductString.append(end);

		std::cout << it->first <<"+" << it->second << ";";
	}

	// send message to server and wait for response
	Product product(ProductString, customerID);
	AddMessage msg(product);
	if (api.sendMessage(msg)) {
		// get response
		std::unique_ptr<Message> msgr = api.recvMessage();
		AddResponseMessage& resp = (AddResponseMessage&)(*msgr); //attempt to make order
		Cart.clear();
		//if (resp.status == MESSAGE_STATUS_OK) {
		//	std::cout << std::endl << "   \"" << product << "\" added successfull." << std::endl;
		//}
		//else {
		//	std::cout << std::endl << "Make Order Failed: " << resp.info << std::endl;
		//}
	}
	return true;
	std::cout << std::endl;
}

void sayGoodbye(ProductLibAPI &api) {
	GoodbyeMessage msg;
	api.sendMessage(msg);
}

void RejectedReadout(cpen333::process::socket &socket) {
	// if we open the socket successfully, continue
	if (socket.open()) {
		std::cout << "connected." << std::endl;

		// create API handler
		JsonProductLibraryApi api(std::move(socket));

		cpen333::process::mutex mutex("Reject");
		std::unique_lock<cpen333::process::mutex> memLock(mutex);
		memLock.unlock();

		// receive message


		std::unique_ptr<Message> msg = api.recvMessage();
		mutex.lock();

		// continue while we don't have an error
		while (msg != nullptr) {
			// react and respond to message
			MessageType type = msg->type();
			switch (type) {
				case MessageType::ADD: {
					AddMessage &add = (AddMessage &)(*msg);
					// add product to order
					Product message = add.product; //newproduct -> Customer ID, Array of items
					std::cout << message.product;
					break;
				}
				default: {
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

int main() {
	// start client
	cpen333::process::socket socket("localhost", PRODUCTSERVERPORT);
	std::cout << "Connecting to Amazoom...";
	std::cout.flush();
	
	
	cpen333::process::mutex mutex("RejectionMap");


	// if we open the socket successfully, continue
	if (socket.open()) {
		std::cout << "connected." << std::endl;

		// create API handler
		JsonProductLibraryApi api(std::move(socket));

		std::thread rejectionresponse(RejectedReadout,std::ref(socket)); //pass in socket

		std::vector<Product> CurrentList;
		std::vector<Product> Cart;
		std::map<Product, int> Order;

		//Update the list of items available
		CurrentList = UpdateItemList(api); 

		//InitalizeMap
		for (const auto& product : CurrentList) {
			Order.insert(std::pair<Product,int>(product, 0));
		}
		
		// keep reading commands until the user quits
		char cmd = 0;
		while (cmd != CLIENT_QUIT) {
			print_menu();

			// get menu entry
			std::cin >> cmd;
			// gobble newline
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::string result;
			std::string count;

			switch (cmd) {
			case CLIENT_ADD:
				do_add(CurrentList, Cart);
				break;
			case CLIENT_REMOVE:
				do_remove(CurrentList,Cart);
				break;
			case CLIENT_SEARCH:
				CurrentList=do_search(api);
				break;
			case CLIENT_ORDER:
				makeorder(api, Cart, Order);
				break;
			case CLIENT_QUIT:
				sayGoodbye(api);
				break;
			default:
				std::cout << "Invalid command number " << cmd << std::endl << std::endl;
			}
			/*cpen333::pause();*/
		}
	}
	else {
		std::cout << "failed." << std::endl;
	}

	return 0;
}

#ifndef ORDER
#define ORDER

#include <vector>
//int POISON = 123123;
/**
* Basic order information containing a customer id and item id
*/
struct Order {
	int customer_id;
	//int item_id;
	std::vector<int> item;
	//std::map<int, int> ItemBasket;

	bool operator==(const Order& other) const {
		return ((customer_id == other.customer_id)
			&& (item == other.item));
	}

	bool operator!=(const Order& other) const {
		return !(*this == other);
	}
};

#endif

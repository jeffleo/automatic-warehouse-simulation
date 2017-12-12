/**
 *@file
 *
 * This file contains the definition of a Song in our database
 *
 */

#ifndef PRODUCT
#define PRODUCT

#include <string>
#include <iostream>

class Product {
public:
	const std::string product;
	const std::string price;

	Product(const std::string product, const std::string& price) :
		product(product), price(price) {}

	std::string toString() const {
		std::string out = product;
		out.append(" - ");
		out.append(price);
		return out;
	}

	friend bool operator<(const Product& a, const Product& b) {
		if (a.product < b.product) {
			return true;
		}
		else if (a.product > b.product) {
			return false;
		}
		return a.price < b.price;
	}

	// equal-to operator for comparisons, both product and price must match
	friend bool operator==(const Product& a, const Product& b) {
		return (a.product == b.product) && (a.price == b.price);
	}

	// not-equal-to operator for comparisons
	friend bool operator!=(const Product& a, const Product& b) {
		return !(a == b);
	}

	// overloaded stream operator for printing
	//    std::cout << song
	friend std::ostream& operator<<(std::ostream& os, const Product& s) {
		os << s.toString();
		return os;
	}
};

#endif //PRODUCTLIB

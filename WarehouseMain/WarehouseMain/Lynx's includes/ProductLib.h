/**
* @file
*
* This contains the data structure for storing the music library locally in memory.
*
*/
#ifndef PRODUCTLIB
#define PRODUCTLIB

#include "Product.h"
#include <vector>
#include <set>
#include <regex>

// Stores a list of products
class ProductLib {
	// private vector
	std::set<Product> product_;
	
public:

	/**
	* Adds a product to the music library
	* @param product product info to add
	* @return true if added, false if already exists
	*/
	bool add(const Product& product) {
		// try to add element to the set
		auto elem = product_.insert(product);
		return elem.second;
	}

	/**
	* Adds products to the music library
	* @param products product info to add
	* @return number of products added
	*/
	size_t add(const std::vector<Product>& products) {
		size_t count = 0;

		for (const Product& product : products) {
			if (add(product)) {
				++count;
			}
		}

		return count;
	}

	/**
	* Removes a product from the music library
	* @param product product info to remove
	* @return true if removed, false if not in library
	*/
	bool remove(const Product& product) {

		//=================================
		// TODO: Remove product from database
		//=================================
		int count = product_.erase(product);
		if (count == 1) 
			return true;
		else 
			return false;
	}

	/**
	* Removes products from the music library
	* @param products product info to remove
	* @return number of products removed
	*/
	size_t remove(const std::vector<Product>& products) {
		size_t count = 0;

		//==================================
		// TODO: Remove products from database
		//==================================
		for (const Product& product : products) {
			count += product_.erase(product);
		}

		return count;
	}

	/**
	* Finds products in the database matching title and artist expressions
	* @param artist_regex artist regular expression
	* @param title_regex title regular expression
	* @return set of products matching expressions
	*/
	std::vector<Product> find(const std::string& name_regex,
		const std::string& price_regex) const {
		std::vector<Product> out;

		//=====================================================
		// TODO: Modify to also include title_regex in search
		//=====================================================

		// compile regular expressions
		std::regex nregex(name_regex);
		std::regex pregex(price_regex);

		// search through products for titles and artists matching search expressions
		for (const auto& product : product_) {
			if (std::regex_search(product.product, nregex) && std::regex_search(product.price, pregex)) {
				out.push_back(product);
			}
		}
		return out;
	}

	/**
	* Retrieves the unmodifiable list of products
	* @return internal set of products
	*/
	const std::set<Product>& products() const {
		return product_;
	}
};

#endif //LAB4_MUSIC_LIBRARY_H


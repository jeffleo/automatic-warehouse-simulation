/**
* @file
*
* This file provides all the JSON encoding/decoding functionality
*
*/

#ifndef LAB4_MUSIC_LIBRARY_JSON_H
#define LAB4_MUSIC_LIBRARY_JSON_H

#include "Product.h"
#include "ProductMessage.h"

#include <json.hpp>   // json parsing

#include <vector>
#include <memory>     // for std::unique_ptr
#include <set>

// convenience alias for json
using JSON = nlohmann::json;

// types of messages
#define MESSAGE_ADD "add"
#define MESSAGE_ADD_RESPONSE "add_response"
#define MESSAGE_REMOVE "remove"
#define MESSAGE_REMOVE_RESPONSE "remove_response"
#define MESSAGE_SEARCH "search"
#define MESSAGE_SEARCH_RESPONSE "search_response"
#define MESSAGE_GOODBYE "goodbye"

// other keys
#define MESSAGE_TYPE "msg"
#define MESSAGE_STATUS "status"
#define MESSAGE_INFO "info"
#define MESSAGE_SEARCH_RESULTS "results"
#define MESSAGE_PRODUCT "product"
#define MESSAGE_PRODUCT_NAME "Product"
#define MESSAGE_PRODUCT_PRICE "Price"
#define MESSAGE_PRODUCT_NAME_REGEX "product_regex"
#define MESSAGE_PRODUCT_PRICE_REGEX "price_regex"

/**
* Handles all conversions to and from JSON
*/
class JsonConverter {
public:
	/**
	* Converts the product to a JSON object
	* @param product product to jsonify
	* @return JSON object representation
	*/
	static JSON toJSON(const Product &product) {
		JSON j;
		j[MESSAGE_PRODUCT_NAME] = product.product;
		j[MESSAGE_PRODUCT_PRICE] = product.price;
		return j;
	}

	/**
	* Converts a vector of products to a JSON array of objects
	* @param products vector of products to jsonify
	* @return JSON array representation
	*/
	static JSON toJSON(const std::vector<Product> &products) {
		JSON j;
		for (const auto& product : products) {
			j.push_back(toJSON(product));
		}
		return j;
	}

	/**
	* Converts a vector of products to a JSON array of objects
	* @param products vector of products to jsonify
	* @return JSON array representation
	*/
	static JSON toJSON(const std::set<Product> &products) {
		JSON j;
		for (const auto& product : products) {
			j.push_back(toJSON(product));
		}
		return j;
	}

	/**
	* Converts an "add" message to a JSON object
	* @param add message
	* @return JSON object representation
	*/
	static JSON toJSON(const AddMessage &add) {
		JSON j;
		j[MESSAGE_TYPE] = MESSAGE_ADD;
		j[MESSAGE_PRODUCT] = toJSON(add.product);
		return j;
	}

	/**
	* Converts an "add" response message to a JSON object
	* @param add_response message
	* @return JSON object representation
	*/
	static JSON toJSON(const AddResponseMessage &add_response) {
		JSON j;
		j[MESSAGE_TYPE] = MESSAGE_ADD_RESPONSE;
		j[MESSAGE_STATUS] = add_response.status;
		j[MESSAGE_INFO] = add_response.info;
		j[MESSAGE_ADD] = toJSON(add_response.add);
		return j;
	}

	//======================================================
	// TODO: Convert "remove" and response message to JSON
	//======================================================

	/**
	* Converts an "remove" message to a JSON object
	* @param remove message
	* @return JSON object representation
	*/
	static JSON toJSON(const RemoveMessage &remove) {
		JSON j;
		j[MESSAGE_TYPE] = MESSAGE_REMOVE;
		j[MESSAGE_PRODUCT] = toJSON(remove.product);
		return j;
	}

	/**
	* Converts an "remove" response message to a JSON object
	* @param remove_response message
	* @return JSON object representation
	*/
	static JSON toJSON(const RemoveResponseMessage &remove_response) {
		JSON j;
		j[MESSAGE_TYPE] = MESSAGE_REMOVE_RESPONSE;
		j[MESSAGE_STATUS] = remove_response.status;
		j[MESSAGE_INFO] = remove_response.info;
		j[MESSAGE_REMOVE] = toJSON(remove_response.remove);
		return j;
	}

	/**
	* Converts a "search" message to a JSON object
	* @param search message
	* @return JSON object representation
	*/
	static JSON toJSON(const SearchMessage &search) {
		JSON j;
		j[MESSAGE_TYPE] = MESSAGE_SEARCH;
		j[MESSAGE_PRODUCT_NAME_REGEX] = search.product_regex;
		j[MESSAGE_PRODUCT_PRICE_REGEX] = search.price_regex;
		return j;
	}

	/**
	* Converts a "search" response message to a JSON object
	* @param search_response message
	* @return JSON object representation
	*/
	static JSON toJSON(const SearchResponseMessage &search_response) {
		JSON j;
		j[MESSAGE_TYPE] = MESSAGE_SEARCH_RESPONSE;
		j[MESSAGE_STATUS] = search_response.status;
		j[MESSAGE_INFO] = search_response.info;
		j[MESSAGE_SEARCH] = toJSON(search_response.search);
		j[MESSAGE_SEARCH_RESULTS] = toJSON(search_response.results);
		return j;
	}

	/**
	* Converts a "goodbye" message to a JSON object
	* @param goodbye message
	* @return JSON object representation
	*/
	static JSON toJSON(const GoodbyeMessage &goodbye) {
		JSON j;
		j[MESSAGE_TYPE] = MESSAGE_GOODBYE;
		return j;
	}

	/**
	* Converts a message to a JSON object, automatically detecting the type
	* @param message
	* @return JSON object representation, {"status"="ERROR", "info"=...} if not recognized
	*/
	static JSON toJSON(const Message &msg) {

		//=============================================================
		// TODO: Convert "remove" and its response to JSON
		//=============================================================

		switch (msg.type()) {
		case ADD: {
			return toJSON((AddMessage &)msg);
		}
		case ADD_RESPONSE: {
			return toJSON((AddResponseMessage &)msg);
		}
		case REMOVE: {
			return toJSON((RemoveMessage &)msg);
		}
		case REMOVE_RESPONSE: {
			return toJSON((RemoveResponseMessage &)msg);
		}
		case SEARCH: {
			return toJSON((SearchMessage &)msg);
		}
		case SEARCH_RESPONSE: {
			return toJSON((SearchResponseMessage &)msg);
		}
		case GOODBYE: {
			return toJSON((GoodbyeMessage &)msg);
		}
		default: {

		}
		}

		// unknown message type
		JSON err;
		err[MESSAGE_STATUS] = MESSAGE_STATUS_ERROR;
		err[MESSAGE_INFO] = std::string("Unknown message type");
		return err;
	}

	/**
	* Converts a JSON object representing a product to a Product object
	* @param j JSON object
	* @return Product
	*/
	static Product parseProduct(const JSON &j) {
		return Product(j[MESSAGE_PRODUCT_NAME], j[MESSAGE_PRODUCT_PRICE]);
	}

	/**
	* Converts a JSON array representing a list of products to a
	* vector of Product objects
	* @param jproducts JSON array
	* @return resulting vector of Product
	*/
	static std::vector<Product> parseProducts(const JSON &jproducts) {
		std::vector<Product> out;

		for (const auto& product : jproducts) {
			out.push_back(parseProduct(product));
		}

		return out;
	}

	/**
	* Converts a JSON object representing an AddMessage to a AddMessage object
	* @param j JSON object
	* @return AddMessage
	*/
	static AddMessage parseAdd(const JSON &jadd) {
		Product product = parseProduct(jadd[MESSAGE_PRODUCT]);
		return AddMessage(product);
	}

	/**
	* Converts a JSON object representing an AddResponseMessage to an AddResponseMessage object
	* @param j JSON object
	* @return AddResponseMessage
	*/
	static AddResponseMessage parseAddResponse(const JSON &jaddr) {
		AddMessage add = parseAdd(jaddr[MESSAGE_ADD]);
		std::string status = jaddr[MESSAGE_STATUS];
		std::string info = jaddr[MESSAGE_INFO];
		return AddResponseMessage(add, status, info);
	}

	//======================================================
	// TODO: Parse "remove" and response message from JSON
	//======================================================
	/**
	* Converts a JSON object representing an RemoveMessage to a RemoveMessage object
	* @param j JSON object
	* @return RemoveMessage
	*/
	static RemoveMessage parseRemove(const JSON &jremove) {
		Product product = parseProduct(jremove[MESSAGE_PRODUCT]);
		return RemoveMessage(product);
	}

	/**
	* Converts a JSON object representing an RemoveResponseMessage to an RemoveResponseMessage object
	* @param j JSON object
	* @return RemoveResponseMessage
	*/
	static RemoveResponseMessage parseRemoveResponse(const JSON &jremover) {
		RemoveMessage remove = parseRemove(jremover[MESSAGE_REMOVE]);
		std::string status = jremover[MESSAGE_STATUS];
		std::string info = jremover[MESSAGE_INFO];
		return RemoveResponseMessage(remove, status, info);
	}

	/**
	* Converts a JSON object representing a SearchMessage to a SearchMessage object
	* @param j JSON object
	* @return SearchMessage
	*/
	static SearchMessage parseSearch(const JSON &jsearch) {
		std::string artist_regex = jsearch[MESSAGE_PRODUCT_NAME_REGEX];
		std::string title_regex = jsearch[MESSAGE_PRODUCT_PRICE_REGEX];
		return SearchMessage(artist_regex, title_regex);
	}

	/**
	* Converts a JSON object representing a SearchResponseMessage to a SearchResponseMessage object
	* @param j JSON object
	* @return SearchResponseMessage
	*/
	static SearchResponseMessage parseSearchResponse(const JSON &jsearchr) {
		SearchMessage search = parseSearch(jsearchr[MESSAGE_SEARCH]);
		std::vector<Product> results = parseProducts(jsearchr[MESSAGE_SEARCH_RESULTS]);
		std::string status = jsearchr[MESSAGE_STATUS];
		std::string info = jsearchr[MESSAGE_INFO];
		return SearchResponseMessage(search, results, status, info);
	}

	/**
	* Converts a JSON object representing a GoodbyeMessage to a GoodbyeMessage object
	* @param j JSON object
	* @return GoodbyeMessage
	*/
	static GoodbyeMessage parseGoodbye(const JSON &jbye) {
		return GoodbyeMessage();
	}

	/**
	* Detects the message type from a JSON object
	* @param jmsg JSON object
	* @return message type
	*/
	static MessageType parseType(const JSON &jmsg) {
		std::string msg = jmsg[MESSAGE_TYPE];
		if (MESSAGE_ADD == msg) {
			return MessageType::ADD;
		}
		else if (MESSAGE_ADD_RESPONSE == msg) {
			return MessageType::ADD_RESPONSE;
		}
		else if (MESSAGE_REMOVE == msg) {
			return MessageType::REMOVE;
		}
		else if (MESSAGE_REMOVE_RESPONSE == msg) {
			return MessageType::REMOVE_RESPONSE;
		}
		else if (MESSAGE_SEARCH == msg) {
			return MessageType::SEARCH;
		}
		else if (MESSAGE_SEARCH_RESPONSE == msg) {
			return MessageType::SEARCH_RESPONSE;
		}
		else if (MESSAGE_GOODBYE == msg) {
			return MessageType::GOODBYE;
		}
		return MessageType::UNKNOWN;
	}

	/**
	* Parses a Message object from JSON, returning in a smart pointer
	* to preserve polymorphism.
	*
	* @param jmsg JSON object
	* @return parsed Message object, or nullptr if invalid
	*/
	static std::unique_ptr<Message> parseMessage(const JSON &jmsg) {

		//=============================================================
		// TODO: Add parsing of "remove" and its response
		//=============================================================

		MessageType type = parseType(jmsg);
		switch (type) {
		case ADD: {
			return std::unique_ptr<Message>(new AddMessage(parseAdd(jmsg)));
		}
		case ADD_RESPONSE: {
			return std::unique_ptr<Message>(new AddResponseMessage(parseAddResponse(jmsg)));
		}
		case REMOVE: {
			return std::unique_ptr<Message>(new RemoveMessage(parseRemove(jmsg)));
		}
		case REMOVE_RESPONSE: {
			return std::unique_ptr<Message>(new RemoveResponseMessage(parseRemoveResponse(jmsg)));
		}
		case SEARCH: {
			return std::unique_ptr<Message>(new SearchMessage(parseSearch(jmsg)));
		}
		case SEARCH_RESPONSE: {
			return std::unique_ptr<Message>(new SearchResponseMessage(parseSearchResponse(jmsg)));
		}
		case GOODBYE: {
			return std::unique_ptr<Message>(new GoodbyeMessage(parseGoodbye(jmsg)));
		}
		}

		return std::unique_ptr<Message>(nullptr);
	}

};

#endif //LAB4_MUSIC_LIBRARY_JSON_H

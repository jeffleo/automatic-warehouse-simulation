/**
 * @file
 *
 * This file contains all message-related objects, independent of the specific API
 *
 * This middle layer allows us to abstract away many of the communication details,
 * allowing us to focus on the core functional implementation.
 *
 */
#ifndef PRODUCTLIBMESSAGES
#define PRODUCTLIBMESSAGES

#include "Product.h"
#include <string>
#include <vector>

/**
 * Types of messages that can be sent between client/server
 */
enum MessageType {
  ADD,
  ADD_RESPONSE,
  REMOVE,
  REMOVE_RESPONSE,
  SEARCH,
  SEARCH_RESPONSE,
  GOODBYE,
  UNKNOWN
};

// status messages for response objects
#define MESSAGE_STATUS_OK "OK"
#define MESSAGE_STATUS_ERROR "ERROR"

/**
 * Base class for messages
 */
class Message {
 public:
  virtual MessageType type() const = 0;
};

class ResponseMessage : public Message {
 public:
  const std::string status;
  const std::string info;
  ResponseMessage(const std::string& status,
                  const std::string& info="") :
      status(status), info(info){}

};

/**
 * Add a product to the library
 */
class AddMessage : public Message {
 public:
  const Product product;

  AddMessage(const Product& product)  : product(product) {}

  MessageType type() const {
    return MessageType::ADD;
  }
};

/**
 * Response to adding a product to the library
 */
class AddResponseMessage : public ResponseMessage {
 public:
  const AddMessage  add;

  AddResponseMessage(const AddMessage& add, std::string status, std::string info = "") :
      ResponseMessage(status, info), add(add) {}

  MessageType type() const {
    return MessageType::ADD_RESPONSE;
  }
};

/**
 * Remove a product from the library
 */
class RemoveMessage : public Message {
 public:
  const Product product;

  RemoveMessage(const Product& product) : product(product) {}

  MessageType type() const {
    return MessageType::REMOVE;
  }
};

/**
 * Response to removing a product from the library
 */
class RemoveResponseMessage : public ResponseMessage {
 public:
  const RemoveMessage remove;

  RemoveResponseMessage(const RemoveMessage& remove, std::string status, std::string info = "") :
      ResponseMessage(status, info), remove(remove) {}

  MessageType type() const {
    return MessageType::REMOVE_RESPONSE;
  }
};

/**
 * Search the library using regular expressions
 */
class SearchMessage : public Message {
 public:
  const std::string product_regex;
  const std::string price_regex;

  SearchMessage(const std::string& product_regex, const std::string& price_regex) :
	  product_regex(product_regex), price_regex(price_regex) {}

  MessageType type() const {
    return MessageType::SEARCH;
  }
};

/**
 * Response to a library search
 */
class SearchResponseMessage : public ResponseMessage {
 public:
  const SearchMessage search;
  const std::vector<Product> results;

  SearchResponseMessage(const SearchMessage& search, const std::vector<Product>& results,
    const std::string& status, const std::string& info = "" ) :
      ResponseMessage(status, info), search(search), results(results) {}

  MessageType type() const {
    return MessageType::SEARCH_RESPONSE;
  }
};

/**
 * Goodbye message
 */
class GoodbyeMessage : public Message {
 public:
  MessageType type() const {
    return MessageType::GOODBYE;
  }
};

#endif //LAB4_MUSIC_LIBRARY_MESSAGES_H

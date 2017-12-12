#ifndef LAB4_MUSIC_LIBRARY_API_JSON_H
#define LAB4_MUSIC_LIBRARY_API_JSON_H

#include "ProductLibApi.h"
#include "ProductMessage.h"
#include "JsonProductConverter.h"

#include <cpen333/process/socket.h>

#include <algorithm> // for std::min

// fixed port for server
#define PRODUCTSERVERPORT 52134
#define WAREHOUSESERVERPORT 39546
#define REJECTIONFEEDBACK 12897

/**
* Handles communication between sockets
*/
class JsonProductLibraryApi : public ProductLibAPI {
private:
	cpen333::process::socket socket_;

	// Fixed message type
	//   NOTE: constants like this don't actually have a memory address,
	//         so they can only be passed by value
	static const char JSON_ID = 0x55;

	/**
	* Writes the JSON info to the socket, NOT including the JSON byte
	* indicator (for symmetry with the read operation)
	*
	* @param j JSON content
	* @return true if write successful, false otherwise
	*/
	bool sendJSON(const JSON& j) {

		// dump to string
		std::string jsonstr = j.dump();

		// encode JSON size, big endian format
		//   (most-significant byte in buff[0])
		char buff[4];
		size_t size = jsonstr.size() + 1;           // one for terminating zero
		for (int i = 4; i-->0;) {
			// cut off byte and shift size over by 8 bits
			buff[i] = (char)(size & 0xFF);
			size = size >> 8;
		}

		// write contents
		bool success = socket_.write(buff, 4);   // contents size
		success &= socket_.write(jsonstr);        // contents

		return success;
	}

	/**
	* Reads a string consisting of exactly size bytes
	* @param str string to append to
	* @param size number of bytes
	* @return true if successful
	*/
	bool readString(std::string& str, size_t size) {

		const int bufferSize = 256;
		char buff[bufferSize];
		bool success = false;

		//======================================================
		// TODO: read and append to str in chunks of 256 bytes
		//======================================================
		size_t amountwritten = 0;
		while (amountwritten < size) {
			int blocksize = std::min<size_t>(size - amountwritten, bufferSize); //if size - amount written is bigger than 256 bits, then read only 256 bits with blocksize
			if (!socket_.read_all(buff, blocksize)) {
				return false;
			}
			str.append(buff, blocksize);
			amountwritten += blocksize; //Update the amount of data written into the str through append
		}
		return true;
	}

	/**
	* Reads and populates a JSON message
	* Assumes the initial JSON indicator byte has already been read, which
	* is why we are now in this method
	*
	* @param jout JSON object to populate
	* @return true if successful, false if error
	*/
	bool recvJSON(JSON& jout) {

		// receive 4-byte size
		unsigned char buff[4];
		if (!socket_.read_all(buff, 4)) {
			return false;
		}

		//=================================================
		// TODO: Decode 4-byte big-endian integer size
		//=================================================

		size_t size = (buff[0] << 24) | (buff[1] << 16) | (buff[2] << 8) | buff[3]; //Largest byte is the smallest index - large endian

		// read entire JSON string
		std::string str;
		if (!readString(str, size)) {
			return false;
		}

		// parse JSON
		jout = JSON::parse(str);

		return true;
	}

	// prevent default constructor
	JsonProductLibraryApi();

public:

	/**
	* Main constructor, takes ownership of socket
	* @param socket
	*/
	JsonProductLibraryApi(cpen333::process::socket&& socket) :
		socket_(std::move(socket)) {}

	/**
	* Sends a message by writing the data to the socket
	* @param msg message to write
	* @return true if successful, false if error
	*/
	bool sendMessage(const Message& msg) {
		JSON jmsg = JsonConverter::toJSON(msg);

		// write single JSON byte
		char id = JSON_ID;
		if (!socket_.write(&id, 1)) {
			return false;
		}
		// write JSON content
		return sendJSON(jmsg);
	}

	/**
	* Reads a message from the socket.  The returned message is
	* contained within a smart pointer to preserve polymorphism
	* and automatically handle freeing of memory resources.  The
	* returned smart pointer can be used similarly to a real
	* pointer, except that it cannot be copied.  You can, however,
	* access members by using the -> operator, and dereference it
	* using the * operator.
	*
	* @return parsed message, nullptr if an error occurred
	*/
	std::unique_ptr<Message> recvMessage() {

		// parse first byte, ensure it is of JSON type
		char id;
		if (!socket_.read_all(&id, 1) || id != JSON_ID) {
			return nullptr;
		}

		// if it is a JSON string, parse into a message
		JSON jmsg;
		if (!recvJSON(jmsg)) {
			return nullptr;
		}

		return JsonConverter::parseMessage(jmsg);
	}

};

#endif //LAB4_MUSIC_LIBRARY_API_JSON_H


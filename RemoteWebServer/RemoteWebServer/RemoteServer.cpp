/**
* @file
*
* This is the main server process.  When it starts it listens for clients.  It then
* accepts remote commands from client
*
*/

#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <memory>
#include <mutex>
#include <cpen333/process/socket.h>


////#include "MusicLibrary.h"
////#include "JsonMusicLibraryApi.h"
//
///**
//* Main thread function for handling communication with a single remote
//* client.
//*
//* @param lib shared library
//* @param api communication interface layer
//* @param id client id for printing messages to the console
//*/
//void service(MusicLibrary &lib, MusicLibraryApi &&api, int id) {
//	std::mutex mutex_;
//
//	//=========================================================
//	// TODO: Implement thread safety
//	//=========================================================
//
//	std::cout << "Client " << id << " connected" << std::endl;
//
//	// receive message
//	std::unique_ptr<Message> msg = api.recvMessage();	//behaves exactly like a regular pointer except it cannot be copied (it can only be moved)
//
//														// continue while we don't have an error
//	while (msg != nullptr) {
//
//		// react and respond to message
//		MessageType type = msg->type();
//		switch (type) {
//		case MessageType::ADD: {
//			// process "add" message
//			// get reference to ADD
//			AddMessage &add = (AddMessage &)(*msg);
//			std::cout << "Client " << id << " adding song: " << add.song << std::endl;
//
//			// add song to library
//			bool success = false;
//			{
//				std::lock_guard<std::mutex> mylock(mutex_);
//				success = lib.add(add.song);
//			}
//
//			// send response
//			if (success) {
//				api.sendMessage(AddResponseMessage(add, MESSAGE_STATUS_OK));
//			}
//			else {
//				api.sendMessage(AddResponseMessage(add, MESSAGE_STATUS_ERROR, "Song already exists in database"));
//			}
//			break;
//		}
//		case MessageType::REMOVE: {
//			//====================================================
//			// TODO: Implement "remove" functionality							DONE
//			//====================================================
//
//			// process "remove" message
//			// get reference to remove
//			RemoveMessage &rem = (RemoveMessage &)(*msg);
//			std::cout << "Client " << id << " removing song: " << rem.song << std::endl;
//
//			// remove song to library
//			bool success = false;
//			{
//				std::lock_guard<std::mutex> mylock(mutex_);
//				success = lib.remove(rem.song);
//
//			}
//
//			// send response
//			if (success) {
//				api.sendMessage(RemoveResponseMessage(rem, MESSAGE_STATUS_OK));
//			}
//			else {
//				api.sendMessage(RemoveResponseMessage(rem, MESSAGE_STATUS_ERROR, "Song doesn't exists in database"));
//			}
//
//			break;
//		}
//		case MessageType::SEARCH: {
//			// process "search" message
//			// get reference to SEARCH
//			SearchMessage &search = (SearchMessage &)(*msg);
//
//			std::cout << "Client " << id << " searching for: "
//				<< search.artist_regex << " - " << search.title_regex << std::endl;
//
//			// search library
//			std::vector<Song> results;
//			{
//				std::lock_guard<std::mutex> mylock(mutex_);
//				results = lib.find(search.artist_regex, search.title_regex);
//			}
//			// send response
//			api.sendMessage(SearchResponseMessage(search, results, MESSAGE_STATUS_OK));
//
//			break;
//		}
//		case MessageType::GOODBYE: {
//			// process "goodbye" message
//			std::cout << "Client " << id << " closing" << std::endl;
//			return;
//		}
//		default: {
//			std::cout << "Client " << id << " sent invalid message" << std::endl;
//		}
//		}
//
//		// receive next message
//		msg = api.recvMessage();
//	}
//}
//
//
//void acceptnewclient(cpen333::process::socket & movedclient, MusicLibrary& lib, int& idx) {
//	JsonMusicLibraryApi api(std::move(movedclient));
//	// service client-server communication
//	service(lib, std::move(api), idx);
//
//}

void tempthread(cpen333::process::socket & movedclient, int& idx) {

}

/*
listen for incoming connections on a specified port
when a client attempts to connect, accept the connection
create a new thread to service the connection and pass (or move) the socket to that thread
detach the service thread so it can continue while the server waits for the next connection
*/


int main() {
	//Mutex mutex_(MUSIC_MUTEX);


	// start server
	cpen333::process::socket_server server(51202);			//may need to change this port number if you get an error about the port already being in use.
	server.open();
	std::cout << "Server started on port " << server.port() << std::endl;


	//===============================================================
	// Allows multiple client-server connections			
	//		- 'Accept' a socket client
	//       - Create an API wrapper around the socket
	//       - Send the API wrapper to the service(...) function to run in a new detached thread
	//===============================================================

	// accept multiple client method
	std::vector<cpen333::process::socket> clients;
	cpen333::process::socket client;

	int i = 0;
	while (server.accept(client)) {

		std::thread(acceptnewclient, std::move(client), std::ref(lib), std::ref(i)).detach();


		// must move to avoid creating copies, which would create multiple write/reader of socket/clients
		// service client-server communication
		i++;
		std::cout << "SERVER ACCEPTED CLIENT " << i << std::endl;
	}


	//std::cin.get();
	// close server
	
	server.close();

	return 0;
}
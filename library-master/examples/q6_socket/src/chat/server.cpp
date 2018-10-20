#include <cpen333/process/socket.h>
#include <mutex>
#include <thread>
#include <iostream>
#include <cstring>

#include "chat.h"

//  Sockets allow for two-way communication over a network.  Like pipes, sockets
//  use a client-server model so that multiple sockets can be established with the same
//  server.
//
//  The server listens for connections at a certain network address on a certain "port".  The port
//  is like an apartment unit number at an address.  It is reminiscent of large telephone switch-boards
//  that used to have physical ports to connect callers, the socket port is used to connect the
//  two processes, allowing them to establish a communication channel.
//
//  In this chat program we demonstrate a very simple communication API.  Before each message, we send
//  a command to the server to indicate what type of message it is.


//  This method will be spun-off into a thread to handle communication with a client
//  NOTE: notice the && symbol on the input pipe.  This signals a "move" operator.
//        Sockets have important internal information which cannot be duplicated.
//        Otherwise, we may introduce race conditions of two sockets trying to read/write
//        to the same end of the channel, and one may inadvertently close the socket while
//        the other is still using it.  This prevents us from using regular assignments like
//              cpen333::process::socket new_socket = other_socket;
//        We *could* pass the socket by reference, but that would mean that the server
//        would need to keep a copy in a vector or array to ensure the socket still
//        exists while we are communicating.
//
//        The alternative is to pass the entire socket over to this method, including
//        ownership of it.  This is one of the purposes of the move operator.
//              cpen333::process::socket new_socket = std::move(other_socket);
//        When we do this, the contents of other_socket are "moved" to new_socket.  The
//        other_socket is then in an indeterminate state and should be discarded.
void service(cpen333::process::socket&& socket) {

  // mutex for protecting print statements
  static std::mutex mutex;

  // read buffer
  static const int buffsize = 1024;
  char buff[buffsize+1];  // extra for terminating \0
  buff[buffsize] = 0;

  // client's name
  std::string name = "";

  // continue reading until the socket disconnects
  size_t read = 0;

  // read a single byte, corresponding to the command
  while ((read  = socket.read(buff, 1)) > 0) {

    // The first byte is a "command", either
    //    - a "hello" which is followed by a name
    //    - a "msg" which is a message from the client
    //    - a "goodbye" which tells us the client is leaving
    if (buff[0] == chat::CMD_HELLO) {
      read = socket.read(buff, buffsize);
      if (read > 0) {
        // get name
        buff[read] = 0;
        name = buff;

        mutex.lock();
        std::cout << "  " << name << " has joined." << std::endl;
        mutex.unlock();
      }
    } else if (buff[0] == chat::CMD_MSG) {
      read = socket.read(buff, buffsize);
      if (read > 0) {
        // get message
        buff[read] = 0;
        mutex.lock();
        std::cout << name << ": \t" << buff << std::endl;
        mutex.unlock();
      }
    } else if (buff[0] == chat::CMD_GOODBYE) {
      mutex.lock();
      std::cout  << "  " << name << " has left." << std::endl;
      mutex.unlock();
      break;
    }
  } // next command

}

int main() {

  // start the server, choose a free port
  cpen333::process::socket_server server(0);
  server.open();

  // obtain the port number and print the address path for our
  // local machine, trying to find an IP address the client
  // can connect to
  int port = server.port();
  std::vector<std::string> address = server.address_lookup();
  std::cout << "List of addresses: " << std::endl;
  for (const auto& add : address) {
    std::cout << "  " << add << std::endl;
  }
  std::cout << std::endl;

  // print best guess of address and port number
  std::cout << "Address: " << address.back() << std::endl;
  std::cout << "Port:    " << port << std::endl;
  std::cout << "Server is running..." << std::endl << std::endl;

  // continuously wait for client connections
  while(true) {
    cpen333::process::socket client;
    if (server.accept(client)) {
      // "move" client to thread
      std::thread thread(service, std::move(client));
      // client at this point is in an indeterminate state and
      //   should be discarded

      // detach thread to let it run on its own
      thread.detach();
    }
  }

  return 0;
}
#include <cpen333/process/socket.h>
#include <limits>
#include "chat.h"

// Simple chat client using sockets
int main() {

  // address and port to connect to
  std::string server;
  int port;

  std::cout << "Enter address: ";
  std::cin >> server;
  std::cout << "Enter port:    ";
  std::cin >> port;
  std::cout << std::endl;

  // ignore the newline after we read a port, otherwise will
  //  interfere with reading a name
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');

  {
    // create a socket and connect to server
    cpen333::process::socket client(server, port);
    client.open();
    std::cout << "Connected to server." << std::endl << std::endl;

    // say hello to server
    std::string msg;
    std::cout << "Enter your name: ";
    std::getline(std::cin, msg);
    client.write(&chat::CMD_HELLO, 1);
    client.write(msg);

    std::cout << std::endl << "Enter \"quit\" to exit" << std::endl << std::endl;

    // send messages to server
    do {
      std::cout << "Enter message: ";
      std::getline(std::cin, msg);

      // if message is "quit", send goodbye command
      if (msg == "quit") {
        client.write(&chat::CMD_GOODBYE, 1);
        break;
      }

      // otherwise, write a message
      client.write(&chat::CMD_MSG, 1);
      client.write(msg);
    } while (true);

    client.close();
  }

  cpen333::pause();
  return 0;
}
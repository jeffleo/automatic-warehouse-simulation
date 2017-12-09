#include <iostream>
#include <thread>

#include <cpen333/process/socket.h>

//  Sockets allow for two-way communication over a network.  Like pipes, sockets
//  use a client-server model so that multiple sockets can be established with the same
//  server.
//
//  The server listens for connections at a certain network address on a certain "port".  The port
//  is like an apartment unit number at an address.  It is reminiscent of large telephone switch-boards
//  that used to have physical ports to connect callers, the socket port is used to connect the
//  two processes, allowing them to establish a communication channel.

// The server will be started on the local machine at the given port number.  If the port
// supplied is zero, then the server will choose a free port, and the input port (passed
// by reference) will be updated to the new value.
void server(int& port) {

  // start server and determine port number
  cpen333::process::socket_server server(port);
  server.open();
  port = server.port();
  std::cout << "server opened port " << port << std::endl;

  // create a client and wait for a connection
  cpen333::process::socket client;
  server.accept(client);

  // send messages back and forth
  std::string msg = "Hello, world!";
  std::cout << "server sending: " << msg << std::endl;
  client.write(msg);

  // close client and server
  client.close();
  server.close();
}

// The client will also be spun off in a new thread, and will
// connect to the given server address and port.
void client(const std::string& server, int& port) {
  // wait for port to be assigned
  std::cout << "Waiting for port...." << std::endl;
  while (port == 0) {
    std::this_thread::yield();
  }
  std::cout << "Port " << port << " assigned" << std::endl;

  // create client and connect to socket
  std::cout << "connecting to port: " << port << std::endl;
  cpen333::process::socket client(server, port);
  client.open();

  // read messages from server
  const int buffsize = 512;
  char buff[buffsize];
  size_t len = 0;
  do {
    len = client.read(buff, buffsize);
    std::string msg(buff, 0, len);
    if (len > 0) {
      std::cout << "client received: " << msg << std::endl;
    }
  } while (len > 0);
  client.close();
}

int main() {

  // start server at a free port
  int port = 0;
  std::thread svr(server, std::ref(port));

  // the server address localhost refers to the local machine
  std::thread cl(client, "localhost", std::ref(port));

  // wait for client and server to finish
  svr.join();
  cl.join();

  return 0;
}
#include <cpen333/process/pipe.h>
#include <iostream>

// A simple client
int main() {

  // creates and opens a named pipe
  cpen333::process::pipe pipe("q4_pipes_server");
  pipe.open();

  // write a couple of messages
  pipe.write("Hello server!");
  pipe.write("What's up?");

  // wait for keyboard input
  cpen333::pause();

  // close
  std::cout << "Goodbye" << std::endl;
  pipe.close();

  return 0;
}
#include <iostream>
#include <thread>
#include <string>

#include "common.h"
#include <cpen333/process/pipe.h>
#include <cpen333/process/subprocess.h>
#include <cpen333/util.h> // to test stdin

//
//  When we are communicating with multiple child processes using pipes, we have two options:
//      Continuously poll the pipes to see if new information has arrived
//      Create a separate thread for each pipe to read the data
//  Here we take the polling approach
//

int main() {

  const int NUM_PIPES = 3;

  std::cout << "Parent process creating the pipes...." << std::endl;
  std::cout << "Type 'Q' to exit main thread" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(2));

  cpen333::process::basic_pipe* pipes[NUM_PIPES];
  cpen333::process::subprocess* processes[NUM_PIPES];

  // create pipes and processes
  for (int i=0; i<NUM_PIPES; ++i) {
    std::string pipe_name = std::string(PIPES_MULTIPLE_PREFIX) + std::to_string(i+1);
    pipes[i] = new cpen333::process::basic_pipe(pipe_name);
    pipes[i]->open();
    
    std::vector<std::string> args;
    args.push_back("./child");
    args.push_back(std::to_string(i+1));
	processes[i] = new cpen333::process::subprocess(args, true, true);
  }

  // main loop, polling pipes
  while(true) { // poll forever
    int data;

    // loop through each pipe checking if there is enough data to read
    for (int i=0; i<NUM_PIPES; ++i) {
      if (pipes[i]->available() >= sizeof(data)) {
        pipes[i]->read(&data);
        std::cout << "Parent read " << data << " from pipe " << i << std::endl;
      }
    }

    // check for keyboard input
    if (cpen333::test_stdin() != 0) {
      char c;
      if (std::cin.get(c)) {
        std::cout << "Parent read " << c << " from keyboard." << std::endl;
        if (c == 'Q' || c == 'q') {
          std::cout << "Shutting down...." << std::endl;
          break;
        }
        std::cout << "Type 'Q' to exit main thread" << std::endl;
      } else {
        break;
      }
    }
  }

  // close all pipes
  for (int i=0; i<NUM_PIPES; ++i) {
    std::cout << "Closing pipe " << (i+1) << std::endl;
    pipes[i]->close();
    pipes[i]->unlink();
    delete pipes[i];
  }

  // clean up processes
  for (int i=0; i<NUM_PIPES; ++i) {
    processes[i]->join();
    delete processes[i];
  }

  cpen333::pause();

  return 0;
}

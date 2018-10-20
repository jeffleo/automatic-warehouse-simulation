#include <iostream>
#include <string>
#include <thread>

#include "common.h"
#include <cpen333/process/pipe.h>
#include <cpen333/process/subprocess.h>
#include <cpen333/process/unlinker.h>
#include <cpen333/util.h> // to test stdin

//
//  When we are communicating with multiple child processes using basic pipes, we have two options:
//      Continuously poll the pipes to see if new information has arrived
//      Create a separate thread for each pipe to read the data
//  Here we take the separate threads approach
//

// non-atomic boolean for termination
volatile bool done = false;

void thread_consumer(int id, cpen333::process::basic_pipe& pipe) {
  // processes should eventually see when done flag is set
  pipe.open();
  while (!done) {
    int data;
    pipe.read(&data);
    std::cout << "Parent thread " << id << ": Read " << data << " from pipe " << id << std::endl;
  }
  pipe.close();  // close pipe
}

int main() {

  const int NUM_PIPES = 3;

  std::cout << "Parent process creating the pipes...." << std::endl;
  std::cout << "Type 'Q' to exit main thread" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(2));

  cpen333::process::basic_pipe* pipes[NUM_PIPES];
  cpen333::process::subprocess* processes[NUM_PIPES];
  std::thread* threads[NUM_PIPES];

  // create pipes, threads, and processes
  // NOTE: we want the read pipe to exist before the child subprocess start,
  //       otherwise the child may see a closed pipe on its first write attempt
  for (int i=0; i<NUM_PIPES; ++i) {
    std::string pipe_name = std::string(PIPES_MULTIPLE_PREFIX) + std::to_string(i+1);
    pipes[i] = new cpen333::process::basic_pipe(pipe_name);
    threads[i] = new std::thread(&thread_consumer, i+1, std::ref(*pipes[i]));

    std::vector<std::string> args;
    args.push_back("./child");
    args.push_back(std::to_string(i+1));
    processes[i] = new cpen333::process::subprocess(args, true, true);
  }

  // main loop
  char c;
  while(std::cin.get(c)) {
    std::cout << "Parent read " << c << " from keyboard." << std::endl;
    if (c == 'Q' || c == 'q') {
      std::cout << "Shutting down...." << std::endl;
      break;
    }
    std::cout << "Type 'Q' to exit main thread" << std::endl;
  }

  // signal complete
  done = true;

  for (int i=0; i<NUM_PIPES; ++i) {
    threads[i]->join();    // wait for thread and delete, will close pipe
    delete threads[i];
    pipes[i]->unlink();    // wait for pipe (must be after thread since thread is using it)
    delete pipes[i];
    processes[i]->join();  // wait for process to finish
    delete processes[i];
  }

  std::cout << "Done." << std::endl;

  cpen333::pause();

  return 0;
}

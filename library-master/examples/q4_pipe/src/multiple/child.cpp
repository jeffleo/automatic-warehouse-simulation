#include "common.h"
#include <cpen333/process/pipe.h>

#include <iostream>
#include <cstdlib>
#include <thread>

int main(int argc, char* argv[]) {

  // detect id from command-line
  int id = 0;
  if (argc > 1) {
    id = atoi(argv[1]);
  }

  std::string pipe_name = std::string(PIPES_MULTIPLE_PREFIX) + std::to_string(id);

  std::cout << "Child " << id << " process opening the pipe \"" << pipe_name << "\"..." << std::endl;
  cpen333::process::basic_pipe pipe(pipe_name);
  pipe.open();

  int start = id*10000;
  for (int i=start; i<start+20; ++i) {
    std::cout << "Child " << id << " writing " << i << " to the pipe" << std::endl;
    if (!pipe.write(i)) {
      std::cout << "Pipe is closed :(, child " << id << " is exiting" << std::endl;
      break;
    }
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  pipe.close();

  return 0;
}
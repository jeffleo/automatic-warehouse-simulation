#include <string>
#include <iostream>
#include <cpen333/process/event.h>

int main(int argc, char* argv[]) {

  if (argc < 4) {
    std::cout << "Not enough arguments, needed 3: clock_event_name, id, # drips" << std::endl;
    return -1;
  }

  std::string event_name = argv[1];
  int id = atoi(argv[2]);
  int drips = atoi(argv[3]);

  cpen333::process::event tic(event_name); // create named event

  for (int i = 0; i < drips; ++i) {
    tic.wait();
    std::cout << "Faucet " << id << ": drip" << std::endl;
  }

  return 0;
}
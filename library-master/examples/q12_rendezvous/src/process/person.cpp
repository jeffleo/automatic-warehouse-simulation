#include <iostream>
#include <thread>
#include <cpen333/process/rendezvous.h>

int main(int argc, char* argv[]) {

  // grab input arguments
  std::string rdv_name = "roller_coaster_rendezvous";
  std::string name = "Judith";
  int time = 1000;
  if (argc > 1) {
    rdv_name = argv[1];
  }
  if (argc > 2) {
    name = argv[2];
  }
  if (argc > 3) {
    time = atoi(argv[3]);
  }

  std::cout << name << " arrived at roller coaster" << std::endl;

  // open rendezvous
  cpen333::process::rendezvous rendezvous(rdv_name, 1);  // default rdv of 1

  for(int i = 0; i < 10; i ++)	{
    std::this_thread::sleep_for(std::chrono::milliseconds(time));  // sleep for id seconds
    std::cout << name << " sat down in cart ....." << std::endl;
    rendezvous.wait();                                            // wait at the rendezvous point
    std::cout << name << ": WEEEEEEEEeeeeee e e e  e  e    e!"  << std::endl;
  }

  return 0;
}
#include <iostream>
#include <cstdlib>   // atoi
#include <thread>
#include <cpen333/process/condition.h>

int main(int argc, char* argv[]) {

  // get event name and id from arguments (if exists)
  std::string event_name = "safe_to_walk";
  int id = 0;
  if (argc > 1) {
    event_name = argv[1];
  }
  if (argc > 2) {
    id = atoi(argv[2]);
  }


  // pedestrian condition
  cpen333::process::condition walk(event_name);

  // record current time for termination condition
  typedef std::chrono::steady_clock clock;
  auto start_time = clock::now();

  // loop for 45 seconds
  while(std::chrono::duration_cast<std::chrono::seconds>(clock::now()-start_time).count() < 45) {
    std::this_thread::sleep_for(std::chrono::milliseconds(600*id));		// sleep for a time period
    walk.wait() ;				     // wait for walk sign
    std::cout << "P" << id << " ";   // show that I have walked
    std::cout.flush();
  }

  return 0;
}
#include <iostream>
#include <thread>    // for "sleep" function

#include "factory.h"
#include <cpen333/process/event.h>

int main() {

  // load events
  cpen333::process::event chassis_ready(CHASSIS_READY_EVENT);
  cpen333::process::event robot_finished(ROBOT_FINISHED_EVENT);

  // make 10 cars
  for (int i=0; i<10; ++i) {
    std::cout << "Production Line: Waiting for next chassis" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "Production Line: New Chassis Arrived" << std::endl;
    chassis_ready.notify_one();  // signal one robot that a chassis is ready
    robot_finished.wait();       // wait until robot is done
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }

  return 0 ;

}
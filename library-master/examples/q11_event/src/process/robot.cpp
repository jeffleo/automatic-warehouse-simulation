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
    std::cout << "Robot: Waiting for Next Chassis" << std::endl;
    chassis_ready.wait();   // wait for next chassis
    std::cout << "Robot: Assembling Chassis" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));   // delay while we assemble car
    std::cout << "Robot: Finished Assembly" << std::endl;
    robot_finished.notify_one();  // robot has finished assembling car
  }

  return 0 ;

}
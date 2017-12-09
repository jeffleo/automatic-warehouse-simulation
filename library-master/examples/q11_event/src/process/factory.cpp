#include <iostream>
#include <thread>
#include <cpen333/process/subprocess.h>

#include "factory.h"
#include <cpen333/util.h>
#include <cpen333/process/event.h>
#include <cpen333/process/unlinker.h>


//
//   An event is just like a "condition", except that it auto-resets once other threads/processes pass through.  It also
//   has the option to either let one thread pass, notify_one(), or all currently waiting threads pass, notify_all().
//


int main() {

  // create events and set to unlink when this process terminates
  cpen333::process::event chassis_ready(CHASSIS_READY_EVENT);
  cpen333::process::event robot_finished(ROBOT_FINISHED_EVENT);

  // make sure to unlink both named resources, the chassis event and robot event
  cpen333::process::unlinker<decltype(chassis_ready)> cunlink(chassis_ready);
  cpen333::process::unlinker<decltype(robot_finished)> runlink(robot_finished);


  std::cout << "Factory started..." << std::endl;

  // create a production line and robot, wait for them to finish
  cpen333::process::subprocess robot("./robot");
  
  std::this_thread::sleep_for(std::chrono::seconds(1));  // robot must start waiting first

  cpen333::process::subprocess production_line("./production_line");

  // wait for production line and robot to finish
  production_line.join();
  robot.join();

  std::cout << "Done." << std::endl;

  cpen333::pause();

  return 0;

}
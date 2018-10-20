#include <iostream>
#include <thread>
#include <cpen333/util.h>
#include <cpen333/process/subprocess.h>
#include <cpen333/process/condition.h>
#include <cpen333/process/unlinker.h>

//
// Conditions allow threads/processes to wait until it is notified to let them pass.  Once notified, the condition will
// continue letting threads/processes pass until the condition is reset.
//
// In this way, Conditions act like a gate.  Once notified, the gate opens to let everyone through.  When reset, the
// gate is closed again, blocking anyone at the door.
//

int main() {

  // conditions
  cpen333::process::condition safetocross("safe_to_walk");
  cpen333::process::condition safetodrive("safe_to_drive");

  // make sure to "unlink" in case of crash
  cpen333::process::unlinker<decltype(safetocross)> crossunlinker(safetocross);
  cpen333::process::unlinker<decltype(safetodrive)> driveunlinker(safetodrive);

  typedef cpen333::process::subprocess process;  // make it easier for us to refer to processes
  process *pedestrians[10]; // 10 pedestrians
  process *cars[10];       // 10 cars

  for(int i = 0; i < 10; i++) {
    std::vector<std::string> pargs;
    pargs.push_back("./pedestrian");
    pargs.push_back("safe_to_walk");
    pargs.push_back(std::to_string(i+1));
    pedestrians[i] = new process(pargs, true, false);

    std::vector<std::string> cargs;
    cargs.push_back("./car");
    cargs.push_back("safe_to_drive");
    cargs.push_back(std::to_string(i+1));
    cars[i]  = new process(cargs, true, false);
  }

  // loop while the intersection is not empty
  bool empty = false;
  while(!empty)	{
    std::cout << std::endl << "Green Light" << std::endl;
    safetodrive.notify();                                   // allow cars to drive over pedestrian crossing
    std::this_thread::sleep_for(std::chrono::seconds(10));  // wait a suitable time delay between lights changing
    safetodrive.reset();                                    // stop cars
    std::cout << std::endl << "Red Light" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));   // wait a suitable time delay between lights changing
    std::cout << std::endl << "Walk on" << std::endl;
    safetocross.notify();                                   // allow pedestrians to cross
    std::this_thread::sleep_for(std::chrono::seconds(15));  // allow pedestrians to cross for 15 seconds
    safetocross.reset();                                    // stop pedestrians
    std::cout << std::endl <<  "No Walking" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));   // wait a suitable time delay between lights changing

    // count remaining processes
    empty = true;  // assume intersection empty
    for (int i=0; i<10; ++i) {
      if (!pedestrians[i]->terminated()) {
        empty = false;
        break;
      }
      if (!cars[i]->terminated()) {
        empty = false;
        break;
      }
    }
  }

  // clean up memory
  for (int i=0; i<10; ++i) {
    delete pedestrians[i];
    delete cars[i];
  }

  std::cout << "Done." << std::endl;

  cpen333::pause();
}
#include <iostream> // for io
#include <thread>   // for sleep

#include <cpen333/util.h>
#include <cpen333/process/subprocess.h>
#include <cpen333/process/rendezvous.h>
#include <cpen333/process/unlinker.h>  // for unlinking shared resource

//
//  A Rendezvous is a place for threads to gather until a certain number arrive, then all are released at once,
//  leading to a (approximately) synchronized running.  In a way, it is like an event that is triggered by everyone
//  arriving. Of course, they won't all run at EXACTLY the same time, since that depends on how many processor cores
//  you have, and the scheduler.
//

int main() {

  // create a shared rendezvous, allowing 6 people
  cpen333::process::rendezvous rendezvous("roller_coaster_rendezvous", 6);
  // we want the rendezvous to be unlinked when it goes out of scope
  cpen333::process::unlinker<cpen333::process::rendezvous> unlinker(rendezvous);

  std::vector<std::string> names;
  names.push_back("Heather");
  names.push_back("Samantha");
  names.push_back("Colleen");
  names.push_back("Sophia");
  names.push_back("Jen");
  names.push_back("Joan");

  // create 6 child process
  std::vector<cpen333::process::subprocess> people;

  int wait_time=500;
  for (const auto& name : names) {
    std::vector<std::string> args;
    args.push_back("./person");
    args.push_back("roller_coaster_rendezvous");
    args.push_back(name);
    args.push_back(std::to_string(wait_time));

    people.push_back(cpen333::process::subprocess(
        args,
        true,
        true
    ));

    wait_time += 500;
  }

  // wait for all processes to complete
  for (auto& person : people) {
    person.join();
  }

  std::cout << "Done." << std::endl;

  cpen333::pause();

  return 0;
}
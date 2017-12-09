#include <iostream> // for io
#include <thread>
#include <vector>
#include <string>

#include <cpen333/util.h>
#include <cpen333/thread/rendezvous.h>

//
//  A Rendezvous is a place for threads to gather until a certain number arrive, then all are released at once,
//  leading to a (approximately) synchronized running.  In a way, it is like an event that is triggered by everyone
//  arriving. Of course, they won't all run at EXACTLY the same time, since that depends on how many processor cores
//  you have, and the scheduler.
//

void person(cpen333::thread::rendezvous& rendezvous, const std::string& name, int time) {
  std::cout << name << " arrived at roller coaster" << std::endl;

  for(int i = 0; i < 10; i ++)	{
    std::this_thread::sleep_for(std::chrono::milliseconds(time));  // sleep for id seconds
    std::cout << name << " sat down in cart ....." << std::endl;
    rendezvous.wait();                                            // wait at the rendezvous point
    std::cout << name << ": WEEEEEEEEeeeeee e e e  e  e    e!"  << std::endl;
  }

}

int main() {

  // create a shared rendezvous, allowing 6 people
  cpen333::thread::rendezvous rendezvous(6);

  std::vector<std::string> names;
  names.push_back("Heather");
  names.push_back("Samantha");
  names.push_back("Colleen");
  names.push_back("Sophia");
  names.push_back("Jen");
  names.push_back("Joan");

  // create 6 child threads
  std::vector<std::thread> people;

  int wait_time=500;
  for (const auto& name : names) {
    people.push_back(std::thread(&person, std::ref(rendezvous), name, wait_time));
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
#include <iostream>
#include <vector>
#include <thread>

#include <cpen333/util.h>
#include <cpen333/thread/event.h>
#include <cpen333/thread/timer.h>

// thread function, dripping faucet
void faucet(cpen333::thread::event &tic, int id, int drips) {
  for (int i=0; i<drips; ++i) {
    tic.wait();
    std::cout << "Faucet " << id << ": drip" << std::endl;
  }
}

int main() {

  cpen333::thread::event tic;  // "tic" event sent to all waiting on the timer

  // create a bunch of threads waiting on the tic event
  std::vector<std::thread> faucets;
  for (int i=0; i<3; ++i) {
    faucets.push_back(std::thread(&faucet, std::ref(tic), i+1, 15));
  }

  // timer
  cpen333::thread::timer<std::chrono::seconds> clock(std::chrono::seconds(3),
                               [&tic](){
                                 tic.notify_all();  // notify all waiting for a tick
                               });

  clock.start();  // must start the clock!!

  // wait for all threads to finish
  for (auto& faucet : faucets) {
    faucet.join();
  }

  cpen333::pause();

  return 0;
}
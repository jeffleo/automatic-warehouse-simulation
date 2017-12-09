#include "Philosopher.h"
#include "Fork.h"
#include <iostream>
#include <thread>
#include <vector>

#include <cpen333/thread/rendezvous.h>  // rendezvous to all start at the same time

typedef  cpen333::thread::rendezvous rendezvous;
volatile bool quit = false;        // marker for telling philosophers to leave table

/**
 * Pick up left fork then right fork if possible, then eat.  If not possible, drop left fork, think, and try again
 * @param id philosopher id
 * @param left left fork
 * @param right right fork
 */
void diner_thread(int id, Fork* left, Fork* right, rendezvous* rdv) {

  Philosopher phil(id);
  
  // all start at same time
  rdv->wait();

  // eat/think loop
  while (!quit) {
    std::cout << "Philosopher " << phil.id() << " is hungry." << std::endl;
    phil.pickup(left);

    // if all caught between here, may cause livelock
    rdv->wait();

    if (phil.try_pickup(right)) {
      std::cout << "Philosopher " << phil.id() << " eating." << std::endl;
      phil.eat(20);
      phil.release_forks();
    } else {
      std::cout << "  Philosopher " << phil.id() << " can't pick up right fork, dropping left" << std::endl;
      phil.release_forks();
    }
    std::cout << "Philosopher " << phil.id() << " thinking." << std::endl;
    phil.think(30);
  }
}

int main() {

  // start N threads
  const int num_diners = 5;

  // create forks
  std::vector<Fork*> forks;
  for (int i=0; i<num_diners; ++i) {
    forks.push_back(new Fork(i+1));
  }

  // create rendezvous and threads
  rendezvous rdv(num_diners);

  std::vector<std::thread*> threads;
  Fork* right_fork = forks.back();  // first diner has last fork on right
  for (int i=0; i<num_diners; ++i) {
    Fork* left_fork = forks[i];
    threads.push_back(new std::thread(&diner_thread, i+1, left_fork, right_fork, &rdv));
    right_fork = left_fork;         // one person's left fork is the next person's right fork
  }

  // let them try to eat for a while
  std::this_thread::sleep_for(std::chrono::seconds(30));

  // Try to quit everyone
  quit = true;  // signal termination
  for (int i=0; i<num_diners; ++i) {
    threads[i]->join();
    delete threads[i];
    delete forks[i];
  }

  return 0;
}
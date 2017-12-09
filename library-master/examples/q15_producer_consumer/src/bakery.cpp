#include <iostream>
#include <thread>

#include "CookieQueue.h"
#include <cpen333/util.h>
#include <cpen333/process/subprocess.h>

//
//  The Producer-Consumer problem deals with having to simultaneously produce "things" or "widgets" into a queue
//  or buffer, while others are consuming them.  The producer needs to stop producing when the queue is full, and the
//  consumer needs to stop consuming when the queue is empty.  This all needs to happen in a synchronized way as well,
//  maximizing throughput.
//


int main() {

  CookieQueue queue(COOKIE_QUEUE_NAME, 32);

  // start 3 bakers
  cpen333::process::subprocess baker1("./baker 1 CHOCOLATE_CHIP", true, false);
  cpen333::process::subprocess baker2("./baker 2 OATMEAL_RAISIN", true, false);
  cpen333::process::subprocess baker3("./baker 3 GINGER_SNAP", true, false);

  // start 3 cookie monsters
  cpen333::process::subprocess monster1("./cookie_monster 1", true, true);
  cpen333::process::subprocess monster2("./cookie_monster 2", true, true);
  cpen333::process::subprocess monster3("./cookie_monster 3", true, true);

  // wait for bakers to finish
  baker1.join();
  baker2.join();
  baker3.join();

  // tell monsters there will be no more cookies, one for each monster
  queue.push(Cookie(CookieType::POISON, -1, -1));
  queue.push(Cookie(CookieType::POISON, -1, -1));
  queue.push(Cookie(CookieType::POISON, -1, -1));

  // wait for monsters to finish
  monster1.join();
  monster2.join();
  monster3.join();

  queue.unlink(); // unlink names to free them on POSIX systems

  std::cout << "Goodbye." << std::endl;
  cpen333::pause();

  return 0;
}
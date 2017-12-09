#include "CookieQueue.h"

#include <thread>
#include <iostream>

int main(int argc, char* argv[]) {

  // detect baker id from command line
  int baker_id = 0;
  if (argc > 1) {
    baker_id = atoi(argv[1]);
  }

  // detect cookie type from command line
  CookieType type = CookieType::CHOCOLATE_CHIP;
  if (argc > 2) {
    std::string ctype = argv[2];   // grab type as string
    if (ctype.compare("CHOCOLATE_CHIP") == 0) {
      type = CookieType::CHOCOLATE_CHIP;
    } else if (ctype.compare("GINGER_SNAP") == 0) {
      type = CookieType::GINGER_SNAP;
    } else if (ctype.compare("OATMEAL_RAISIN")) {
      type = CookieType::OATMEAL_RAISIN;
    }
  }

  CookieQueue queue(COOKIE_QUEUE_NAME, 256);

  // add cookies to queue in loop
  for (int i=0; i<500; ++i) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));  // take time to make cookie

    queue.push(Cookie(type, baker_id, i));
  }

  std::cout << "Baker " << baker_id << " finished." << std::endl;

  return 0;
}
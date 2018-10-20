#include <iostream>
#include <string>
#include <thread>

#include "BarberShop.h"

int main(int argc, char* argv[]) {

  // grab name from argv
  std::string name = "Customer";
  if (argc > 1) {
    name = argv[1];
  }

  // load barbershop
  BarberShop barbershop(BARBER_SHOP_NAME, BARBER_SHOP_CHAIRS);

  // keep going until barbershop is closed
  while(true) {
    std::cout << "Customer " << name << " arriving for haircut" << std::endl;
    if (!barbershop.customerWaitsForHaircut()) {
      std::cout << name << " turned away :(" << std::endl;
      if (!barbershop.isOpened()) {
        break;  // leave if shop is now closed
      }
    } else {
      std::cout << name << "'s haircut is complete" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  }

  return 0;
}
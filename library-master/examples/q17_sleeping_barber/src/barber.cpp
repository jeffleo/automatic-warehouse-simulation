
#include <string>

#include "BarberShop.h"

#include <iostream>
#include <string>
#include <thread>

int main(int argc, char* argv[]) {

  // grab name from argv
  std::string name = "Barber";
  if (argc > 1) {
    name = argv[1];
  }

  // load barbershop
  BarberShop barbershop(BARBER_SHOP_NAME, BARBER_SHOP_CHAIRS);

  std::cout << "Barber " << name << " arriving at work" << std::endl;
  for (int i=0; i<100; ++i) {
    if (!barbershop.barberWaitsForCustomer()) {
      std::cout << "Shop closed" << std::endl;
      break;
    }
    std::cout << name << " serving customer" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
  }
  std::cout << name << " leaving for the night" << std::endl;

  return 0;
}
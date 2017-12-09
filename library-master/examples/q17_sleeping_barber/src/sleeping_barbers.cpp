#include <string>
#include <thread>

#include "BarberShop.h"
#include <cpen333/process/subprocess.h>
#include <cpen333/process/condition.h>

int main() {

  // barbershop
  cpen333::process::condition barbershop_opened(BARBER_SHOP_NAME);
  barbershop_opened.reset();  // in case was open last time

  cpen333::process::subprocess barbershop("./barber_shop", true, true);
  barbershop_opened.wait();  // wait until shop opened

  // a couple barbers
  cpen333::process::subprocess barber1("./barber Michael", true, true);
  cpen333::process::subprocess barber2("./barber Michelle", true, true);

  // a bunch of customers
  cpen333::process::subprocess customer1("./customer Hank", true, true);
  cpen333::process::subprocess customer2("./customer Homer", true, true);
  cpen333::process::subprocess customer3("./customer Henry", true, true);
  cpen333::process::subprocess customer4("./customer Hope", true, true);
  cpen333::process::subprocess customer5("./customer Heather", true, true);
  //  cpen333::process::subprocess customer6("./customer Holly", true, true);
  //  cpen333::process::subprocess customer7("./customer Hubert", true, true);
  //  cpen333::process::subprocess customer8("./customer Helen", true, true);
  //  cpen333::process::subprocess customer9("./customer Hugh", true, true);
  //  cpen333::process::subprocess customer10("./customer Hayden", true, true);

  // purposely leave processes open so you can add your own via command-line
  return 0;
}
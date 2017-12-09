#include "cpen333/os.h"

#include <iostream>

#ifdef POSIX
#include <semaphore.h>
#include <unistd.h>
#include <cerrno>    // error numbers
#include <string>

void __sem_unlink(const char* name) {
  int status = sem_unlink(name);
  if (status != 0 && errno == EACCES) {
    std::cerr << "Failed to unlink semaphore " << name << ": ACCESS DENIED" << std::endl;
  }
}

int main(int argc, char* argv[]) {

  // command-line arguments
  if (argc > 2) {
    // try to unlink all semaphores provided as arguments
    for (int i=1; i<argc; ++i) {
      __sem_unlink(argv[i]);
    }
  }
  // read from pipe
  else if (!isatty(fileno(stdin))) {
    std::string name;
    while (std::cin >> name) {
      __sem_unlink(name.c_str());
    }
  }
  // print usage
  else {
    std::cout << "Usage:" << std::endl;
    std::cout << argv[0] << " <list of semaphore IDs...>" << std::endl;
    std::cout << "\tSemaphore IDs can be found on OSX by using the command" << std::endl;
    std::cout << "\t\tlsof | grep PSXSEM" << std::endl;
    std::cout << "\tThese can be piped in directly, i.e." << std::endl;
    std::cout << "\t\tlsof | grep PSXSEM | awk '{print $7}' | " << argv[0] << std::endl;
  }

  return 0;
}
#else
int main() {

  std::cin.peek();
  std::cout << "Cannot unlink semaphores on Windows" << std::endl;
  return 0;
}
#endif

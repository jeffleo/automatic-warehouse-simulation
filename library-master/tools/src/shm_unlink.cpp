#include "cpen333/os.h"
#include <iostream>


#ifdef POSIX
#include <sys/mman.h>
#include <unistd.h>
#include <cerrno>    // error numbers
#include <string>

void __shm_unlink(const char* name) {
  int status = shm_unlink(name);
  if (status != 0 && errno == EACCES) {
    std::cerr << "Failed to unlink shared memory " << name << ": ACCESS DENIED" << std::endl;
  }
}

int main(int argc, char* argv[]) {

   // command-line arguments
  if (argc > 2) {
    // try to unlink all semaphores provided as arguments
    for (int i=1; i<argc; ++i) {
      __shm_unlink(argv[i]);
    }
  }
  // read from pipe
  else if (!isatty(fileno(stdin))) {
    std::string name;
    while (std::cin >> name) {
      __shm_unlink(name.c_str());
    }
  }
  // print usage
  else {
    std::cout << "Usage:" << std::endl;
    std::cout << argv[0] << " <list of shared memory IDs...>" << std::endl;
    std::cout << "\tShared memory IDs can be found on OSX by using the command" << std::endl;
    std::cout << "\tlsof | grep PSXSHM" << std::endl;
    std::cout << "\tThese can be piped in directly, i.e." << std::endl;
    std::cout << "\t\tlsof | grep PSXSHM | awk '{print $7}' | " << argv[0] << std::endl;
  }

  return 0;
}
#else
int main() {
  std::cout << "Cannot unlink shared memory on Windows" << std::endl;
  return 0;
}
#endif

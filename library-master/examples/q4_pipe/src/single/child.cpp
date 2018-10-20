#include "common.h"
#include <cpen333/process/pipe.h>

int main() {

  std::cout << "Child process opening pipe....." << std::endl;
  cpen333::process::basic_pipe pipe(PIPE_NAME, 1024);   // create or connect to pipe

  // read an integer from the pipe
  int x;
  pipe.read(&x); // read in to address of x, uses template to deduce type and size
  std::cout << "Child process read integer = " << x << " from pipe....." << std::endl;

  // read all of an array from the pipe
  int array[10];
  pipe.read_all(&array[0], sizeof(array)) ;

  std::cout << "Child process read array = ";
  for(size_t i = 0; i < sizeof(array)/sizeof(array[0]); ++i) {
    std::cout << " " << array[i];
  }
  std::cout << std::endl;

  // read a string until we get a terminating zero
  std::string name;
  char c;
  while (pipe.read(&c) && c != 0) {
    name.push_back(c);
  }
  std::cout << "Child Process read string = \"" << name << "\" from pipe....." << std::endl;

  // read struct
  example ex;
  pipe.read(&ex) ;
  std::cout << "Child Process read struct ex = [" << ex.x  << ", " << ex.y << "] from pipe....." << std::endl;

  std::cout << "Press ENTER to exit" << std::endl;
  std::cin.get();
  
  return 0;
}
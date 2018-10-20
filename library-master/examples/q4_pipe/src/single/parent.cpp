#include <iostream>
#include <thread>

#include "common.h"  // include common information (pipe name, data type)
#include <cpen333/util.h>
#include <cpen333/process/pipe.h>
#include <cpen333/process/unlinker.h>
#include <cpen333/process/subprocess.h>

//
//  Basic pipes allow for one-way communication between two processes in the form of a stream of data bytes.  They often
//  have a fixed memory size.  Reading from a pipe usually "blocks" (or "waits") until the requested number of bytes
//  is available.  Writing to a pipe usually blocks when there is not enough room in the pipe to write the data.  Once
//  a process reads data from the pipe, that data is removed, which leaves more room in the pipe to write more data.
//
//  Since data is read from pipes in the same order it was put in (i.e. the first byte written is the first byte read),
//  they are often referred to as a FIFO (or fifo), which stands for First-In-First-Out.
//
//  Just like for shared memory, we need to give our pipe a text-based name so processes can connect to it.  If the pipe
//  already exists, we will just connect to the existing one.  Otherwise, a new pipe is created.  The pipe also has a
//  persistence associated with it...
//      Windows: the pipe will exist until all references to it are removed
//      Linux/OSX: the pipe will exist until the machine is rebooted (kernel persistence)
//  On POSIX systems, we also have the option to "unlink" the name from the memory, meaning that further attempts to
//  connect to the named resource will create a new, independent pipe.  The old one will be freed when all existing
//  references to it are removed.
//
//  Note that "true" pipes often block (or wait) until there is a connection on both the read-end and the write-end of
//  the pipe.  For portability and ease-of-use, we have created a pseudo-pipe in the file
//      cpen333/process/basic_pipe.h
//  which is created using a circular buffer in shared memory.  This version does not block, nor does it guarantee
//  that only two processes are accessing the pipe.

int main() {

  std::cout << "Parent process creating the pipe....." << std::endl;

  // create a shared pipe of size 1024 bytes
  cpen333::process::basic_pipe pipe(PIPE_NAME, 1024);
  pipe.open();  // open pipe

  // Rather than unlink at the end of this process, we will use RAII principles and create a special
  // object that will unlink the name from the named resource for us.
  cpen333::process::unlinker<decltype(pipe)> unlinker(pipe);    // unlink pipe when runs out of scope

  std::cout << "Parent process creating child to read from pipe....." << std::endl;
  cpen333::process::subprocess p1("./child", true, true);

  // write i to pipe
  int i = 20;
  std::cout << "Press ENTER to write the integer " << i << " to the pipe.....";
  std::cin.get();   // get line
  pipe.write(i);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  // write array to pipe
  int array[10] = {1,2,3,4,5,6,7,8,9,0};
  std::cout << "Press ENTER to write the integer array 1 2 3 4 5 6 7 8 9 0 to the pipe....." << std::endl;
  std::cin.get();
  pipe.write(&array[0], sizeof(array)); // write the array of integers' to the pipe
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  // write string to pipe
  std::string name = "Snuffaluffagus";
  std::cout << "Press ENTER to write the string \"" << name << "\" to the pipe....." << std::endl;
  std::cin.get();
  pipe.write(name.c_str(), name.size()+1);  // write the terminating 0 as well
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  // write struct
  example ex = {10, 2.3456f};
  std::cout << "Press ENTER to write the structure [" << ex.x << ", " << ex.y << "] to the pipe....." << std::endl;
  std::cin.get();
  pipe.write(ex); // uses template-based function to deduce the type/size of data to write
  pipe.close();

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  // wait for process to terminate
  p1.join();

  std::cout << "Done." << std::endl;

  cpen333::pause();

  return 0;
}

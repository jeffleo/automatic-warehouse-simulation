#include <iostream>
#include <cpen333/util.h>
#include <cpen333/process/shared_memory.h>
#include <cpen333/process/subprocess.h>

//
//  Interprocess Shared Memory (a.k.a Shared Memory, Datapools, or Mapped Memory) are simply blocks of shared memory
//  accessible by multiple threads/processes. Since processes do not share memory address space, we need to set up a
//  special memory block that can be mapped to each process.  This is usually done with direct operating system calls.
//  We have made convenience classes for you, found in
//
//      cpen333/process/shared_memory.h
//
//  In particular, two classes may be of interest:
//      cpen333:process::shared_memory              // raw block of memory
//      cpen333::process::shared_object<typename>   // template-based wrapper for direct access to members
//
//  For multiple processes to access the memory, we need to give it a text-based name.  Each process can then
//  connect to the memory block using the given name.  The first time we try to connect to the shared memory block,
//  the OS will create it and initialize it to zeros.  Subsequent connection attempts with the same name will simply
//  connect to the existing memory.  This block of memory will persist until...
//      Windows:    all references to the shared block of memory are removed
//      Linux/OSX:  the machine is rebooted (i.e. kernel persistence)
//  On POSIX systems (i.e. Linux/OSX), we also have the option to manually "unlink" a name from a named resource
//  like a shared memory block.  Once a name is unlinked, the next attempt to connect to a memory block with that
//  name will create an entirely new block of memory.  The old block will then be freed when all references to it
// (those made prior to the block being "unlinked") are removed.
//
//  There is a subtlety in blocks of shared memory: the shared data block MUST be contiguous and completely
//  self-contained.  This means we can't store regular object pointers, since those will inevitably
//  refer to memory outside of the memory block itself.  Other process will not have access to that outside memory.
//
//  Even more subtlety: we also need to avoid containers, and other classes which internally may store data on the
//  heap.  This includes strings, vectors, maps, etc...
//

// Layout of block of shared memory stored within a struct
// Since all members are fixed-length basic types, this is a self-contained contiguous block of memory.  In C++, the
// memory layout is gauranteed, with members arranged in order [ 20 chars, 1 long, 1 double ].  The actual number of
// bytes may change from system to system, but the addresses can be computed using the sizeof() operator
struct SharedData {
  char movie[20];    // title
  long length;       // in seconds
  double rating;     // on a 5* scale
};


int main() {

  // Start by making a shared memory object called "movie1".  If it doesn't exist, the OS will create it.  If it does,
  // it will just attach to it.  We have to specify the size of the memory object in bytes, which we can figure out
  // based on our struct layout and the sizeof() operator.
  std::cout << "Parent attempting to create/use the shared memory....." << std::endl;
  cpen333::process::shared_memory memory("movie1", sizeof(SharedData));

  // To access particular variables within the shared memory, we need a pointer to their location.  The root address
  // of the shared memory block can be obtained via the memory.get() function.  Since the OS doesn't know anything
  // about our memory layout, we have to manually 'cast' it to the actual data type.  The struct's guaranteed memory
  // layout makes this safe.
  SharedData *data = (SharedData*)memory.get();

  std::cout << "Parent linked to memory at address ....." << data << std::endl;

  std::cout << "Parent writing value 'Toy Story 2' to movie variable....." << std::endl;
  // copy data from a string to the output array, leave room for a terminating 0
  std::string("Toy Story 2").copy(data->movie, 19, 0);
  data->movie[19] = 0;

  std::cout << "Parent writing value '95' to length variable....." << std::endl;
  data->length = 95;

  std::cout << "Parent writing value '4.9' to ratings variable....." << std::endl;
  data->rating = 4.9;

  //	Now that we have created the shared memory and have stored data in it, it is safe to create
  //	a child thread that can access the data
  std::cout << "Press Enter to launch child process" << std::endl;
  std::cin.get();

  // spawn child process
  // start it running, and detached
  cpen333::process::subprocess p1("./child Subprocess", true, true);
  p1.join();        // wait for the child process to Terminate


  // Note that our shared memory still exists at this point.  On Windows, the shared memory will be freed when the
  // parent process and all child processes using the memory terminate.  On POSIX systems, however (like Linux or OSX),
  // the shared memory will continue to exist until the next reboot.  This means if you run this program multiple times,
  // it will remember what was written last time.  To prevent this behaviour, we need to explicitly disconnect the
  // shared memory from its name.  We call this "unlinking" it.  This function does nothing on Windows, but should
  // be called anyways for portability (and in case you ever end up programming on linux or mac).
  memory.unlink();  // release shared object's name so memory can be freed when process terminates

  std::cout << "Done." << std::endl;

  cpen333::pause();

  return 0;
}
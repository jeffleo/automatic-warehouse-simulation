#include <cpen333/process/pipe.h>
#include <iostream>
#include <thread>

//  Let's say we want to have 100 processes communicate over a two-way channel
//  with a central process using pipes.  We could accomplish this with 200 named
//  pipes, each one a one-way channel to or from the central process.  This approach
//  doesn't scale well however: the more processes, the more pipe names we will need,
//  and it may start getting hard to keep track of them.  If we want other processes
//  to join dynamically, we may not even know how many pipes we need beforehand.
//
//  Windows named pipes work a bit differently compared to our simple FIFO: they are
//  bi-directionaly communication channels (i.e. two-way) and use a
//  client-server model:
//    - The "pipe server" waits, listening for an incoming connection
//    - When a client connects, a two-way pipe is established
//    - The pipe server is then free to create a second pipe with the same name and
//      wait for a new connection
//  This allows multiple pipes to connect to the server using the same name,
//  and all these pipes can be communicating simultaneously.
//
//  In the course library, we mimick this behaviour on POSIX systems by establishing
//  a single named pipe between the server and a client, unlinking the name so it can be
//  reused, then have the server create a new pipe with the same name to wait for the
//  next connection.

//  This method will be spun-off into a thread to handle communication with a client
//  NOTE: notice the && symbol on the input pipe.  This signals a "move" operator.
//        Pipes have important internal information which cannot be duplicated.
//        Otherwise, we may introduce race conditions of two pipes trying to read/write
//        to the same end of the pipe, and one may inadvertently close the pipe while
//        the other is still using it.  This prevents us from using regular assignments like
//              cpen333::process::pipe new_pipe = other_pipe;
//        We *could* pass the pipe by reference, but that would mean that the server
//        would need to keep a copy in a vector or array to ensure the pipe still
//        exists while we are communicating.
//
//        The alternative is to pass the entire pipe over to this method, including
//        ownership of it.  This is one of the purposes of the move operator.
//              cpen333::process::pipe new_pipe = std::move(other_pipe);
//        When we do this, the contents of other_pipe are "moved" to new_pipe.  The
//        other_pipe is then in an indeterminate state and should be discarded.
void service(cpen333::process::pipe&& pipe, int i) {

  // read buffer
  static const int buffsize = 1024;
  char buff[buffsize+1];  // extra for terminating \0
  buff[buffsize] = 0;

  // keep reading until we get zero bytes back (signalling the pipe is closed)
  size_t nread = 0;

  std::cout << "pipe " << i << " started" << std::endl;

  // When pipes read, they "block", meaning they will wait here for input.
  while ( (nread = pipe.read(buff, buffsize)) > 0 ) {
    buff[nread] = 0; // terminating zero
    if (nread > 0) {
      std::cout << "pipe " << i << ": " << buff << std::endl;
    }
  }

  // The other end of the pipe is now closed, so we will close this one
  // and exit.
  pipe.close();
  std::cout << "pipe " << i << " closed." << std::endl;

}

int main() {

  // create a pipe server and start it
  cpen333::process::pipe_server server("q4_pipes_server");
  server.open();

  // connect to 10 remote processes
  for (int i=0; i<10; ++i) {
    std::cout << "Waiting for pipe " << i << std::endl;

    // create an empty client to be populated by the server
    cpen333::process::pipe client;

    // wait until a client connects
    if (server.accept(client)) {

      // "move" the client to the server thread
      std::thread thread(service, std::move(client), i);

      // at this point "client" is in an indeterminate state
      // it should not be used

      thread.detach();  // leave thread to handle client independently
    }
  }

  // close the server
  server.close();

  return 0;
}
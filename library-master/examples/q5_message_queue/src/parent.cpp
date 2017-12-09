#include <iostream>
#include <vector>
#include <string>
#include <thread>

#include "common.h"
#include <cpen333/util.h>
#include <cpen333/process/subprocess.h>
#include <cpen333/process/message_queue.h>
#include <cpen333/process/unlinker.h>
#include <cpen333/thread/timer.h>  // timer

//
// Message queues are very similar to pipes, but allow for multiple processes to write to them.  They are usually used
// for sending short messages (often a single integer value).
//
// The details of "true" message queues differ between Windows and POSIX systems.
//    On Windows, messages are addressed to individual threads.  Each message is assigned an integer value, and can have
//    two additional integers as supplemental information.  When reading from a Windows message queue, you have the
//    option on "filter" based on a message value range, so instead of processing messages in order, you can pull the
//    next message that satisfies the range requirement.
//
//    On POSIX systems (Linux/OSX), message queues are named resources, just like pipelines and datapools, and messages
//    can be any custom size, though the maximum message size must be specified at creation.  Each message is also
//    assigned an integer priority value.  On the reader's end, the next message to be read is the next highest
//    priority message.
//
// Here, we have made a pseudo-message queue using a circular buffer.  Messages are assumed to have a fixed size,
// corresponding to a templated data type.  The definitions can be found in
//     cpen333/process/message_queue.h
//

// store info about my pen-pal
struct PalInfo {
  std::string name;
  cpen333::process::subprocess*  process;
  cpen333::process::message_queue<MessageType>* mailbox;

  PalInfo(std::string name, cpen333::process::subprocess* process,
          cpen333::process::message_queue<MessageType>* mailbox) :
	  name(name), process(process), mailbox(mailbox) {}
};

int main() {

  // I am Po
  std::string my_name = "Po";
  cpen333::process::message_queue<MessageType> my_mailbox(my_name);
  // unlink resource name when out of scope
  cpen333::process::unlinker<decltype(my_mailbox)> unlink(my_mailbox);

  // These are my friends
  std::vector<std::string> friend_names;
  friend_names.push_back("Tigress");
  friend_names.push_back("Master Shifu");
  friend_names.push_back("Viper");
  friend_names.push_back("Monkey");
  friend_names.push_back("Mantis");
  friend_names.push_back("Crane");
  std::vector<PalInfo> friends;

  // create a bunch of friends and mailboxes
  for (const std::string& pal : friend_names) {

    // build up process command as a vector of strings
    std::vector<std::string> args;
    args.push_back("./penpal");
    args.push_back(pal);

    friends.push_back(PalInfo(
                        pal,
                        new cpen333::process::subprocess(args, true, true),
                        new cpen333::process::message_queue<MessageType>(pal)
                      ));
  }

  // We will send mail at a fixed rate to each of my friends.  This is
  // a message-blast lambda that "captures" the friends vector and my_name
  auto message_blast = [&friends, &my_name]() {
    std::cout << "Blasting message" << std::endl;
    for (PalInfo& pal : friends) {
      pal.mailbox->send( MessageType(MessageCode::NOTE, std::string("Hi ") +  pal.name, my_name) );
    }
  };

  // create timer to send messages using the message-blast method
  cpen333::thread::timer<std::chrono::seconds> timer(std::chrono::seconds(2), message_blast);
  timer.start();

  // create separate thread to read my own messages
  auto message_reader = [&my_name, &my_mailbox]() {
    while(true) {
      MessageType message = my_mailbox.receive();

      // check for poison pill
      if (message.code == MessageCode::GOODBYE) {
        break;
      }

      std::cout << my_name << " recieved message from " << message.sender << ": " << message.msg << std::endl;
    }
  };
  std::thread read_thread(message_reader);

  // Take a nap here for a bit while things run
  std::this_thread::sleep_for(std::chrono::seconds(20));

  timer.stop();  // stop timer


  // In order to close all friend threads, we will send a good-bye message.  This tells them to shut down.
  // send a goodbye to everyone
  for (PalInfo& pal : friends) {
    pal.mailbox->send(MessageType(MessageCode::GOODBYE, " ... ", my_name));
    pal.process->join();    // wait for process to terminate
    pal.mailbox->unlink();  // close down mailbox
    delete pal.mailbox;
    delete pal.process;
  }

  // say goodbye to myself to close my reader thread
  my_mailbox.send(MessageType(MessageCode::GOODBYE, "", my_name));
  read_thread.join();

  std::cout << "Goodbye." << std::endl;

  cpen333::pause();

  return 0;

}

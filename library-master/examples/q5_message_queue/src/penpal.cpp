#include "common.h"
#include <cpen333/process/message_queue.h>

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {

  // grab name from argv if exists
  std::string name = "Tai Lung"; //... wait, is he really my friend?
  if (argc > 1) {
    name = argv[1];
  }
  std::cout << name << " up and running..." << std::endl;

  // create (or attach) to my message queue
  cpen333::process::message_queue<MessageType> mailbox(name);

  // keep waiting for messages until we hear goodbye
  for (;;) {

    MessageType message = mailbox.receive();

    // check if quit message
    if (message.code == MessageCode::GOODBYE) {
      break;
    }

    std::cout << name << " recieved message from " << message.sender << ": " << message.msg << std::endl;
    std::cout << "Sending message back..." << std::endl;

    // extract name of sender into a string
    std::string pal = message.sender;
    // send message back to sender
    cpen333::process::message_queue<MessageType> tomail(pal);
    tomail.send(MessageType(MessageCode::NOTE, std::string("Hello ") + pal + std::string("!"), name));

  }

  return 0;

}
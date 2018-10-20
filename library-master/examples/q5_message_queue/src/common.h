#ifndef MESSAGE_QUEUES_COMMON_H
#define MESSAGE_QUEUES_COMMON_H

#include <string>

// Code for identifying message
enum MessageCode {
  NOTE, GOODBYE
};

// Struct for message layout in memory
struct MessageType {
  MessageCode code;
  char msg[20];     // must use fixed-size array since will be copied to shared memory
  char sender[20];

  // need a default constructor
  MessageType() : code(MessageCode::NOTE) {
    msg[0] = 0;
    sender[0] = 0;
  }

  // convenience constructor to use string
  MessageType(MessageCode code, const std::string& msg, const std::string& sender) : code(code) {
    // copy strings to msg/sender
    size_t i = 0;
    for (; i<sizeof(this->msg)-1 && i < msg.length(); ++i) {
      this->msg[i] = msg[i];
    }
    this->msg[i] = 0; // terminating zero

    for (i=0; i<sizeof(this->sender)-1 && i < sender.length(); ++i) {
      this->sender[i] = sender[i];
    }
    this->sender[i] = 0;  // terminating zero
  }
};

#endif //MESSAGE_QUEUES_COMMON_H

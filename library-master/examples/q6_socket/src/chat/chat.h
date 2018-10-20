#ifndef Q6_SOCKETS_CHAT_H
#define Q6_SOCKETS_CHAT_H

// command bytes
namespace chat {
static const char CMD_HELLO = 0x01;     // hello command, followed by name
static const char CMD_GOODBYE = 0x02;   // goodbye command
static const char CMD_MSG = 0x03;       // msg command
}

#endif //LIBRARY_CHAT_H

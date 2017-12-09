/**
* @file
*
* This is the main server process.  When it starts it listens for clients.  It then
* accepts remote commands from client
*
*/

#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <memory>
#include <mutex>
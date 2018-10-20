#include <fstream>
#include <vector>
#include <iostream>
#include <thread>
#include <string>
#include <cpen333/util.h>
#include <cpen333/thread/timer.h>

//
//  Most operating systems have timers that you can leverage to perform certain tasks.  These timers are generally
//  limited to a single process.  On Windows, you can use a timer to send a message to each thread's message queue.
//  On POSIX, you can use timers to send signal events.  In both cases, you can also specify a callback function
//  to be called when the timer "ticks".
//
//  For inter-process communication, you would have to configure the timer's callback to notify all processes of the
//  event using either datapools or message queues.  We can also use synchronization mechanisms to inform processes
//  of a timer event (such as conditions/events)
//
//  Here we simply demonstrate the use of a timer to ping a callback function at a regular interval.  The timer
//  definition is in
//      cpen333/thread/timer.h
//


// will be used as a callback for our timer
class TimedPrinter {
 public:
  TimedPrinter(const std::vector<std::string>& lines) :
      lidx_(0), lines_(lines) {}

  // override () operator
  void operator()() {
    if (lidx_ < lines_.size()) {
      std::cout << lines_[lidx_];
      std::cout.flush();
      ++lidx_;
      if (lidx_ == lines_.size()) {
        lidx_ = 0;
      }
    }
  }

 private:
  size_t lidx_;   // line index
  std::vector<std::string> lines_;  // list of lines to sing
};

int main() {

  std::vector<std::string> lines;

  // load data from file
  std::ifstream fin("./timer.dat");
  if (!fin.is_open()) {
    fin.open("../timer.dat");  // search up one directory with Visual Studio
  }

  while (fin.is_open() && !fin.eof()) {
    std::string line;
    std::getline(fin, line);

    // substitute "\n" for end-of-line characters
    // and strip away \r characters
    std::string add;
	size_t i;
    for (i=0; i+1<line.size(); ++i) {
      if (line[i]== '\\' && line[i+1] == 'n') {
        add.push_back('\n');
        ++i; // advance another character
      } else if (line[i] != '\r') {
        add.push_back(line[i]);
      }
    }
    if (i < line.size() && line[i] != '\r') {
      add.push_back(line[i]);
    }

    lines.push_back(add);
  }
  fin.close();  // make sure to close file

  // timer set for every half-second, call singer as a callback
  TimedPrinter printer(lines);
  cpen333::thread::timer<std::chrono::milliseconds> timer(std::chrono::milliseconds(500), printer);

  // run for about half a minute, then stop
  timer.start();
  std::this_thread::sleep_for(std::chrono::seconds(34));
  timer.stop();

  std::cout << std::endl << "Done." << std::endl;

  cpen333::pause();

  return 0;
}
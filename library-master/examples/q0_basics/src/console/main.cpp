#include <iostream>
#include <thread>   // for sleep
#include <chrono>   // for times
#include <string>
#include <vector>

#include <cpen333/console.h>
#include <cpen333/util.h>

//  To help creating console-based GUIs, we provide a simple "console" class in
//      cpen333/console.h
//
//  This will demonstrated some of that functionality

void sleep(int ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int main() {

  cpen333::console console;
  typedef cpen333::color color;

  console.set_background_color(color::DARK_MAGENTA);
  console.set_foreground_color(color::WHITE);
  console.clear_display();             // fill entires display
  console.set_cursor_visible(false);   // hide cursor

  console.set_cursor_position(2, 33);
  std::cout << "Goodnight Moon";
  console.set_cursor_position(4, 29);
  std::cout << "By Margaret Wise Brown";
  std::cout.flush();                        // on linux, doesn't flush until newline or manual
  sleep(2000);

  std::string poem[] = {"In the great green room", "There was a telephone", "And a red balloon",
                        "And a picture of", "The cow jumping over the moon",
                        "And there were three little bears", "   sitting on chairs",
                        "And two little kittens", "And a pair of mittens", "And a little toy house",
                        "And a young mouse", "And a comb and a brush", "   and a bowl full of mush",
                        "And a quiet old lady", "   who was whispering \"hush\"", "Goodnight room",
                        "Goodnight moon", "Goodnight cow jumping over the moon", "Goodnight light",
                        "And the red balloon", "Goodnight bears", "Goodnight chairs", "Goodnight kittens",
                        "And goodnight mittens", "Goodnight clocks", "And goodnight socks",
                        "Goodnight little house", "And goodnight mouse", "Goodnight comb",
                        "And goodnight brush", "Goodnight nobody", "Goodnight mush",
                        "And goodnight to the old lady", "   whispering \"hush\"", "Goodnight stars",
                        "Goodnight air", "Good night noises everywhere"};

  // initialize row/column locations
  int rtop = 7;
  int r = rtop;
  int c = 4;

  for (const std::string& str : poem) {
    console.set_cursor_position(r, c);
    //console.clear_line(); // over-write
    std::cout << str;

    // special rows
    size_t idx = str.find("red");
    if (idx != std::string::npos) {
      console.set_cursor_position(r, (int)idx+c);
      console.set_foreground_color(color::RED);
      std::cout << "RED";
      console.set_foreground_color(color::WHITE);
    } else {
      idx = str.find("green");
      if (idx != std::string::npos) {
        console.set_cursor_position(r, (int)idx+c);
        console.set_foreground_color(color::GREEN);
        std::cout << "GREEN";
        console.set_foreground_color(color::WHITE);
      }
    }
    std::cout.flush();

    sleep(1000);

    ++r;
    if (r >= 26) {
      r = rtop;
      c += 42;
    }
  }

  // always reset console settings, or else they may persist
  //    after your program exits
  console.reset();

  console.set_cursor_position(28, 4);

  cpen333::pause();

  console.clear_all();

  return 0;
}

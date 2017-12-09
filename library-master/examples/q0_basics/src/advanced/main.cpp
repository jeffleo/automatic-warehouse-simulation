#include <iostream>
#include <string>
#include <vector>
#include <cpen333/util.h>

int main() {

  //========================================================
  // ENUM
  //========================================================
  // enums are great for restricting inputs to be one of a certain set of valid items
  enum Priority {LOW, MEDIUM, HIGH};

  Priority priority = LOW;
  switch (priority) {
    case LOW: {
      std::cout << "priority is \"LOW\"" << std::endl;
      break;
    }
    case MEDIUM: {
      std::cout << "priority is \"MEDIUM\"" << std::endl;
      break;
    }
    case HIGH: {
      std::cout << "priority is \"HIGH\"" << std::endl;
      break;
    }
  }

  //========================================================
  // auto
  //========================================================
  // The "auto" specifier (c++11 and higher) deduces the type of a variable based on the assignment.
  // It's not so useful in cases where you want to create a variable, since you probably want
  //   *some* control over how your data is stored.  However, it's extremely useful when accessing
  //   existing variables or outputs of functions, when you may not be so concerned about the type
  auto x = 10.0;                // probably DON'T want to use "auto" here, but it will still work
  std::cout << "x = " <<  x << std::endl << std::endl;

  // "auto" is often used when looping through items
  std::string gocrazy[] =  {"No", "TV", "and", "no", "beer", "makes", "Homer", "something", "something"};
  for (const auto& word : gocrazy) {
    std::cout << word << " ";
  }
  std::cout << std::endl << std::endl;

  //========================================================
  // decltype
  //========================================================
  // The "decltype" specifier (c++11 and higher) determines the declared type of a variable.  It is
  // EXTREMELY useful in template code, and prevents having to change a whole bunch of definitions if
  // you decide to change the type later.

  int y = 0;
  // The following says: whatever type you happened to define "y" as, make a vector of that type and fill it with
  //     100 instances of y.
  // If above you change y to be a double, this next line of code does not need to be changed.
  std::vector<decltype(y)> yvec(100, y);

  //========================================================
  // Lambda Expressions
  //========================================================
  // Lambda expressions create things called "closures", which represent a function along with
  // an "environment" containing items that are accessible within the function.
  //
  // They are defined as:
  //         [list of "captured" variables](list of inputs){ body };
  //
  // The return type of the function is deduced from the return statement in the body

  // define a "less-than" function between two integers
  auto less_than = [](int x, int y) {
    return (x < y);
  };

  bool lt = less_than(10, 20);
  std::cout << "10 less than 20: ";
  if (lt) {
    std::cout << "true" << std::endl;
  } else {
    std::cout << "false" << std::endl;
  }

  // "captured" variables can be used inside the function without needing to be an input
  // The following captures "prefix" by reference
  std::string prefix = "Schwartzenegger: ";

  auto printfun = [&prefix](std::string says){
    std::cout << prefix << says << std::endl;
  };

  printfun("Do it!");
  printfun("I'll be back!");
  printfun("Get to the chopper!");
  printfun("Hasta la vista, baby!");

  cpen333::pause();

  return 0;
}
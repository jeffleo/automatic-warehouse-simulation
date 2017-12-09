#include "Cat.h"
#include <string>

Cat::Cat(std::string name, int age) : Animal(name, age) { // pass the name and age to the parent constructor
  // initialize anything else unique to Cat
}

Cat::~Cat() {
  // clear any dynamic memory unique to Cat
}

// Override "Speak"
std::string Cat::speak() {
  return "meow";
}

// Class of referring type (not overridden, but can be hidden)
std::string Cat::type() {
  return "Cat";
}

std::string Cat::rule() {
  // ... if you are a cat person, I suppose you might think so...
  return "rule";
}
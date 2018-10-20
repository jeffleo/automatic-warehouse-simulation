#include "Animal.h"
#include <string>   // even though Animal.h will automatically include <string>,
                    // it is usually a good idea to include here too.  Its #ifdef guards
                    // will prevent it from actually loading twice.

// Define all our class functions

// Animal constructor
Animal::Animal(std::string name, int age)
  : name_(name), age_(age) // initialze the internal name_ and age_ to the provided name and age
{
  // nothing else to do here
}

// Animal destructor
Animal::~Animal() {
  // delete any dynamic memory here (our animal doesn't have any yet)
}

// Animal Speak
std::string Animal::speak() {
  return "...";  // we don't know what to say
}

// Class of referring type (not overridden, but can be hidden)
std::string Animal::type() {
  return "Animal";
}

// Name "accessor", returns the animal's name
std::string Animal::name() {
  return name_;
}

// Age "accessor", returns the animal's age
int Animal::age() {
  return age_;
}
#ifndef ANIMAL_H  // ifdef "guard" to prevent definitions from loading multiple times
#define ANIMAL_H

#include <string>  // c++ string (which differs from a c-style string that is just an array of chars)

// new "animal" base class
class Animal {
 // private, hidden stuff
 private:
  // for private members, I like to use a post-fixed _ to indicate that they are private
  std::string name_;  // name of the animal
  int age_;           // age of the animal

 // public, accessible stuff
 public:

  Animal(std::string name, int age);    // constructor that passes in name and age
  virtual ~Animal();                    // destructor, destroys any internal data

  // "virtual" functions can be overridden by sub-classes
  virtual std::string speak();          // returns a noise

  // non-virtual functions are not "overridden", but can be "hidden" by sub-classes
  std::string type();

  // "accessor" methods for private member variables
  // so that outside world can see what they are, but cannot change them
  std::string name();                   // returns our name
  int age();                            // returns our age

};

#endif
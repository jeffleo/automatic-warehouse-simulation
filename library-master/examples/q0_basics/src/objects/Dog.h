#ifndef DOG_H
#define DOG_H

#include "Animal.h" // needed for Animal class
#include <string>   // technically not needed, since Animal.h will include <string> itself.
                    // However, it is good practice to include anything that is explicitly
                    // used in THIS file just in case the Animal.h file is ever changed to
                    // remove the include. Its #ifdef guard will prevent it from actually loading
                    // a second time.

// Create a "Dog" class that "inherits" all public things from Animal
//    This means we can use any public member methods/variables,
//    but we do not have DIRECT access to private members (though they do still exist,
//    i.e. a Dog does still have an _age and _name, but we cannot access them directly)
class Dog : public Animal {

 private:

 public:
  Dog(std::string name, int age);   // constructor
  virtual ~Dog();                   // destructor
  virtual std::string speak();      // we will override speak
  std::string type();              // will "hide" Animal::type

  // something unique to dog
  std::string drool();

};

#endif //DOG_H

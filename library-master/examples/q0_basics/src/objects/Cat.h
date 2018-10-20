#ifndef CAT_H
#define CAT_H

#include "Animal.h" // needed for Animal class
#include <string>   // technically not needed, since Animal.h will include <string> itself.
                    // However, it is good practice to include anything that is explicitly
                    // used in THIS file just in case the Animal.h file is ever changed to
                    // remove the include. Its #ifdef guard will prevent it from actually loading
                    // a second time.

// Create a "Cat" class that "inherits" all public things from Animal
//    This means we can use any public member methods/variables,
//    but we do not have DIRECT access to private members (though they do still exist,
//    i.e. a Cat does still have an _age and _name, but we cannot access them directly)
class Cat : public Animal {

 private:

 public:
  Cat(std::string name, int age);   // constructor
  virtual ~Cat();                   // destructor
  virtual std::string speak();      // we will override speak
  std::string type();              // will "hide" Animal::type

  // unique methods to Cats
  std::string rule();

};

#endif //CAT_H

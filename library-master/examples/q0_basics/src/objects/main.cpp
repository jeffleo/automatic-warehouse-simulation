#include <iostream>  // for printing stuff
#include <cpen333/util.h>
#include "Animal.h"  // to reference an Animal
#include "Cat.h"     // to create a Cat
#include "Dog.h"     // to create a Dog

int main() {

  // allocate a cat and dog on the "heap"
  Cat* garfield = new Cat("Garfield", 39);  // Garfield is a really old Cat
  Dog* wishbone = new Dog("Wishbone", 22);  // Wishbone is also super old now :S

  // pointer to an existing animal
  Animal* pet;

  // What do the animals say?
  std::cout << "Testing Cat/Dog pointers" << std::endl;
  std::cout << "  " << garfield->name() << " the " << garfield->type() << " says " << garfield->speak() << std::endl;
  std::cout << "  " << wishbone->name() << " the " << wishbone->type() << " says " << wishbone->speak() << std::endl;

  std::cout << "Testing Animal pointer" << std::endl;
  pet = garfield;
  std::cout << "  " << pet->name() << " the " << pet->type() << " says " << pet->speak() << std::endl;
  // Did it print what you expected?

  // Trying out functions unique to Cat and Dog
  std::cout << wishbone->type() << "s " << wishbone->drool() << " and "
            << garfield->type() << "s " << garfield->rule() << std::endl;

  // Forcing it to get at the parent class' speak() method
  // this is more than what you need to know for this course (and perhaps life)
  std::cout << "  Forcing " << pet->name() << " to say " << pet->Animal::speak() << std::endl;

  // free heap memory
  delete wishbone;
  delete garfield;

  cpen333::pause();
  return 0;
}
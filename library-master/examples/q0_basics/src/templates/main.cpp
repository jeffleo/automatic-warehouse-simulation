#include <iostream>
#include <string>
#include <cpen333/util.h>

//
// Templates allow us to write code more generically, independent of any particular type.
//    Instead, it provides a blueprint for a class or function, where the type can be specified later.
//    Under-the-hood, the template class will be compiled separately for every unique typename used in your code.
//    Different typenames result in completely different classes/methods

/**
 * Holds a single item of type T
 * @tparam T generic type
 */
template<typename T>
class Container {
  T val_;  // contained value

 public:
  Container(const T& val) : val_(val) {} // initialize val_ to val

  /**
   * Gets the contained value
   * @return value by reference
   */
  T& get() {
    return val_;
  }

  /**
   * Sets contained value
   * @param val value to copy
   */
  void set(const T& val) {
    val_ = val;
  }
};


//  There could be multiple template parameters, and we can also define templated functions

/**
 * Prints key-value pair as "key: value"
 * @tparam S type of key
 * @tparam T type of value
 * @param s key
 * @param t value
 */
template<typename S, typename T>
void print_this(const S& s, const T& t) {
  std::cout << s << ": " << t << std::endl;
}

int main() {

  // make a container for an integer
  Container<int> age(20);
  std::cout << "Container age: " << age.get() << std::endl;

  // make a container for double
  Container<double> pi(3.14159);
  std::cout << "Container pi: " << pi.get() << std::endl;

  // make a container for string
  Container<std::string> name("Gunther");
  std::cout << "Container name: " << name.get() << std::endl;

  // print Gunther's age
  print_this<std::string,int>(name.get(), age.get());

  // For template functions, the compiler can be smart about things and try to automatically
  // deduce your template parameters.
  print_this(name.get(), age.get());  // same as above

  print_this("Pi", pi.get());

  std::cout << std::endl;
  print_this("JULIET", "O Romeo, Romeo! Wherefore art thou Romeo?");
  print_this("ROMEO", "Hey Juliet, I'm over here under the balcony");

  cpen333::pause();
  return 0;
}

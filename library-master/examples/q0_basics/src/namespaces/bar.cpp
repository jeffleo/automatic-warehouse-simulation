#include "bar.h"

// other "includes" should generally be outside the namespace, otherwise the namespace
// identifier will be prepended to their own internal namespaces (unless that's exactly what you want).
#include <string>
#include <cmath>   // for cbrt function

// we are defining things in this namespace
namespace bar {

// Note: these are "Doxygen" comment blocks that can be turned into documentation
// with the Doxygen tool.  See http://www.stack.nl/~dimitri/doxygen/index.html
// Many IDEs such as CLion or Eclipse can generate prototypes for them automagically,
// you just fill in the details.
/**
 * Takes the cubed root of a
 * @param a input
 * @return a^(1/3)
 */
double foo(double a) {
  // return std::cbrt(a);
  double sign = 1.0;
  if (a < 0) {
    sign = -1;
    a = -a;
  }
  double out = sign*std::pow(a, 1.0/3);
  return out;
}

/**
 * Computes a^b
 * @param a base
 * @param b exponent
 * @return a^b
 */
int bar(int a, int b) {
  // computes the exponent the inefficient way
  // a smarter implementation would use exponentiation by squaring
  int c = 1;
  for (int i=0; i<b; ++i) {
    c = c*a;
  }

  return c;
}

/**
 * Says hello
 * @return bar::FooBar's hello message
 */
std::string FooBar::hello() {
  return "Hello from bar::FooBar";
}

} // bar
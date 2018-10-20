#include "foo.h"

// other "includes" should generally be outside the namespace, otherwise the namespace
// identifier will be prepended to their own internal namespaces (unless that's exactly what you want).
#include <string>
#include <cmath>   // for sqrt function

// we are defining things in this namespace
namespace foo {

// Note: these are "Doxygen" comment blocks that can be turned into documentation
// with the Doxygen tool.  See http://www.stack.nl/~dimitri/doxygen/index.html
// Many IDEs such as CLion or Eclipse can generate prototypes for them automagically,
// you just fill in the details.
/**
 * Takes the square root of a
 * @param a input
 * @return sqrt(a)
 */
double foo(double a) {
  return sqrt(a);
}

/**
 * Takes the square of a
 * @param a input
 * @return a*a
 */
int bar(int a) {
  return a*a;
}

/**
 * Says hello
 * @return foo::FooBar's hello message
 */
std::string FooBar::hello() {
  return "Hello from foo::FooBar";
}

} // foo
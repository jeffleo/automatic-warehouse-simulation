#ifndef NAMESPACES_BAR_H
#define NAMESPACES_BAR_H

// other "includes" should generally be outside the namespace, otherwise the namespace
// identifier will be prepended to their own internal namespaces (unless that's exactly what you want).
#include <string>

// It is a good idea to use a "namespace" to help group related code together,
// and to prevent "name collisions" (two methods or classes that have the exact same
// name/signature)
// methods/classes in this namespace can be accessed using bar::method or bar::Class
namespace bar {

double foo(double a);

int bar(int a, int b);

class FooBar {
 public:
  std::string hello();
};

} // bar

#endif //NAMESPACES_BAR_H
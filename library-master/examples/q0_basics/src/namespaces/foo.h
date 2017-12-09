#ifndef NAMESPACES_FOO_H
#define NAMESPACES_FOO_H

// other "includes" should generally be outside the namespace, otherwise the namespace
// identifier will be prepended to their own internal namespaces (unless that's exactly what you want).
#include <string>

// It is a good idea to use a "namespace" to help group related code together,
// and to prevent "name collisions" (two methods or classes that have the exact same
// name/signature)
// methods/classes in this namespace can be accessed using foo::method, foo::Class
namespace foo {

double foo(double a);

int bar(int a);

class FooBar {
 public:
  std::string hello();
};

} // bar

#endif //NAMESPACES_FOO_H
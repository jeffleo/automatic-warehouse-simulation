#include <iostream> // for cout
#include <cpen333/util.h>
#include "foo.h"    // foo's stuff
#include "bar.h"    // bar's stuff

// Sometimes you will see something like this:
//    using namespace std;
// This tells the compiler to search the "std" namespace when it cannot find a symbol.
// The benefit of this is you don't have to keep typing things like std::string, std::cout, etc...,
// instead, you just write: string, cout.
// HOWEVER, this is in general a very BAD idea.  It leaves you open to name collisions when using things from multiple
// namespaces.


int main() {
  int a = 10;
  int b = 5;

  // // EXAMPLE OF WHAT __NOT__ TO DO
  // using namespace foo;
  // using namespace bar;
  // // If you uncomment both "using namespace foo;" and "using namespace bar;" above,
  // // which version of foo(double) will be used?  What happens?
  // double c = foo(a);

  // use the foo(double) method within the namespace "foo"
  double d = foo::foo(a);
  std::cout << "foo::foo(" << a << ") = " << d << std::endl;

  // use the foo(double) method within the namespace "bar"
  double e = bar::foo(a);
  std::cout << "bar::foo(" << a << ") = " << e << std::endl;

  int f = foo::bar(a);
  std::cout << "foo::bar(" << a << ") = " << f << std::endl;

  int g = bar::bar(a, b);
  std::cout << "bar::bar(" << a << "," << b << ") = " << g << std::endl;

  // To reduce the need of typing namespaces (especially long ones),
  // you can use a specific, localized "using" directive (c++11 and higher)
  // or typedef (prior to c++11)

  typedef foo::FooBar Foo;         // prior to c++11
  // using Foo = foo::FooBar;      // after c++11
  Foo foo;
  std::cout << foo.hello() << std::endl;

  typedef bar::FooBar Bar;         // prior to c++11
  // using Bar = bar::FooBar;      // after c++11
  Bar bar;
  std::cout << bar.hello() << std::endl;

  cpen333::pause();

  return 0;
}
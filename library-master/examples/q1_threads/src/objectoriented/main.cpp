#include <iostream>
#include <vector>
#include <cpen333/util.h>
#include <cpen333/thread/thread_object.h>  // custom thread-based objects

//
//  You can create custom classes that are descendents of std::thread.  I have made two such classes for you:
//      cpen333::thread::thread_object, and cpen333::thread::suspended_thread_object
//  These expect you to write a method called
//      int main()
//  in your custom class.  The suspended version will start in a "suspended" mode, waiting for you to manually start
//  the thread.
//


// create a custom class with a "main" method that will execute within a thread
class CustomThread1 : public cpen333::thread::thread_object {

 private:
  int id_;

 public:
  CustomThread1(int id) : id_(id) {}

 private:

  // Override "main" method, which is executed by thread
  // We generally want to keep this method "private", so others don't accidentally call it again
  int main() {
    for(int i = 0; i < 30; ++i)	{
      std::cout << "I am a CustomThread1 object/Thread. My id is " << id_ << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    return id_;
  }
};

class CustomThread2 : public cpen333::thread::thread_object {
 private:
  int id_;

 public:
  CustomThread2(int id) : id_(id) {}

 private:
  // override "main" method, which is executed by thread
  int main() {
    for(int i = 0; i < 30; ++i)	{
      std::cout << "I am a CustomThread2 object/Thread. My id is " << id_ << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    return id_;
  }
};

int main() {

  // create instance of the above active object on the stack
  // then allow it to run and then wait for it to terminate
  CustomThread1	object1(1), object2(2), object3(3) ;
  CustomThread2	object4(4), object5(5), object6(6);

  object1.start();
  object2.start();
  object3.start();

  // wait for 2 seconds
  std::this_thread::sleep_for(std::chrono::seconds(2));
  
  // start objects 4/5/6
  object4.start();				// allow thread to run
  object5.start();				// allow thread to run
  object6.start();				// allow thread to run

  // wait for everyone to finish
  int o1 = object1.join() ;
  std::cout << "object 1 finished with: " << o1 << std::endl;
  int o2 = object2.join() ;
  std::cout << "object 2 finished with: " << o2 << std::endl;
  int o3 = object3.join() ;
  std::cout << "object 3 finished with: " << o3 << std::endl;
  int o4 = object4.join() ;
  std::cout << "object 4 finished with: " << o4 << std::endl;
  int o5 = object5.join() ;
  std::cout << "object 5 finished with: " << o5 << std::endl;
  int o6 = object6.join() ;
  std::cout << "object 6 finished with: " << o6 << std::endl;

  std::cout << "Finished." << std::endl;

  cpen333::pause();

  return 0;
}

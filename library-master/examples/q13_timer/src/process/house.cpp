#include <cpen333/util.h>
#include <cpen333/process/event.h>
#include <cpen333/thread/timer.h>
#include <cpen333/process/subprocess.h>
#include <cpen333/process/unlinker.h>

int main() {

  cpen333::process::event tic("clock_tic_event");             // "tic" event sent to all waiting on the timer
  cpen333::process::unlinker<decltype(tic)> unlinker(tic);    // unlink tic's name when runs out of scope

  // create a bunch of threads waiting on the tic event
  std::vector<cpen333::process::subprocess> faucets;
  for (int i=0; i<3; ++i) {
    std::vector<std::string> args;
    args.push_back("./faucet");
    args.push_back("clock_tic_event");
	args.push_back(std::to_string(i+1));
	args.push_back(std::to_string(15));
    faucets.push_back(cpen333::process::subprocess(
        args,
        true,  // start immediately
        true   // run independently
    ));
  }

  // timer
  cpen333::thread::timer<std::chrono::seconds> clock(std::chrono::seconds(3),
                                                     [&tic](){
                                                       tic.notify_all();  // notify all waiting for a tick
                                                     });

  clock.start();  // must start the clock!!

  // wait for all processes to finish
  for (auto& faucet : faucets) {
    faucet.join();
  }

  cpen333::pause();

  return 0;
}
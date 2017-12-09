#ifndef MUTEX_PROCESS_H
#define MUTEX_PROCESS_H

// common names for "named" resources between parent/child
#define MUTEX_SHARED_MEMORY_NAME "mutex_process_shared"
#define MUTEX_MUTEX_NAME "mutex_process_mutex"
#define MUTEX_SEMAPHORE_NAME "mutex_process_semaphore"


// struct definition for shared data
struct SharedData {
  int a;
};

#endif //MUTEX_PROCESS_H

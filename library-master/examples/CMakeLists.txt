# customize output directory
set(MY_OUTPUT_DIR examples)

# Introduction to C++ and Concurrency
add_subdirectory("q0_basics")
add_subdirectory("q1_threads")
add_subdirectory("q2_processes")

# Inter-Process Communication
add_subdirectory("q3_shared_memory")
add_subdirectory("q4_pipe")
add_subdirectory("q5_message_queue")
add_subdirectory("q6_socket")

# Synchronization
add_subdirectory("q7_mutex")
add_subdirectory("q8_semaphore")
add_subdirectory("q9_condition")
add_subdirectory("q10_condition_variable")
add_subdirectory("q11_event")
add_subdirectory("q12_rendezvous")
add_subdirectory("q13_timer")
add_subdirectory("q14_monitor")

# Advanced Synchronization Problems
add_subdirectory("q15_producer_consumer")
add_subdirectory("q16_reader_writer")
add_subdirectory("q17_sleeping_barber")
add_subdirectory("q18_dining_philosophers")
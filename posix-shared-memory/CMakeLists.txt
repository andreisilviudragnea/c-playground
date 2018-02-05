set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

link_libraries(Threads::Threads rt)

add_executable(test_writer test_writer.c common.h ../include/util.h)
add_executable(test_reader test_reader.c common.h ../include/util.h)

configure_file(test.sh test.sh COPYONLY)

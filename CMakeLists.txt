project(encrypt352)
cmake_minimum_required(VERSION 3.1)

find_package(Threads REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_library(vendor STATIC "src/vendor/encrypt-module.c")
target_sources(vendor PRIVATE "src/vendor/encrypt-module.h")
target_link_libraries(vendor PRIVATE Threads::Threads)

add_executable(encrypt352 src/main.c)

target_link_libraries(encrypt352 vendor)

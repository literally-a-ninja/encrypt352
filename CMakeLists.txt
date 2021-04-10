project(encrypt352)
cmake_minimum_required(VERSION 3.1)

find_package(Threads REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_library(vendor STATIC "src/vendor/encrypt-module.c")
target_sources(vendor PRIVATE "src/vendor/encrypt-module.h")

file(GLOB_RECURSE APP_OBJS CONFIGURE_DEPENDS "src/*.c")
file(GLOB_RECURSE APP_HEADS CONFIGURE_DEPENDS "src/*.h")

add_library(app STATIC ${APP_OBJS})
target_link_libraries(app PUBLIC Threads::Threads)

add_executable(encrypt352 src/main.c)
target_link_libraries(encrypt352 vendor app Threads::Threads)

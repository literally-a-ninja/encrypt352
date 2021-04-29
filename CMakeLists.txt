project(encrypt352)
cmake_minimum_required(VERSION 3.1)

find_package(Threads REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)

add_library(fkworks0 INTERFACE)
target_include_directories(fkworks0 INTERFACE src/fkworks0/)

set(LIB_VENDOR_SOURCES 
    src/vendor/coroutines.c src/vendor/encrypt-module.c src/vendor/ring.c src/vendor/global.c
)
set(LIB_VENDOR_HEADERS
    src/vendor/commmon.h src/vendor/coroutines.h
    src/vendor/encrypt-module.h src/vendor/global.h src/vendor/ring.h
)

add_library(vendor STATIC ${LIB_VENDOR_SOURCES})
target_sources(vendor PRIVATE ${LIB_VENDOR_HEADERS})
target_link_libraries(vendor Threads::Threads fkworks0)

add_executable(encrypt352 src/encrypt352.c)
target_link_libraries(encrypt352 PRIVATE vendor fkworks0)

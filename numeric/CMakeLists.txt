cmake_minimum_required(VERSION 3.2)
project(NumLib LANGUAGES CXX VERSION 0.7 DESCRIPTION "A c++ PDE numeric library")

find_package ( Threads REQUIRED )

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_COMPILER g++-7)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

#SRC
set(NUM_SRC numericutils.h numericutils.cpp)

#LIB
add_library(NumLib STATIC ${NUM_SRC})

#EXEC
add_executable(num main.cpp)
target_link_libraries(num Threads::Threads NumLib)
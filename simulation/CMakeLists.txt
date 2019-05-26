cmake_minimum_required(VERSION 3.2)
project(SimLib LANGUAGES CXX VERSION 0.7 DESCRIPTION "A c++ Monte Carlo simulation library")

find_package(Threads REQUIRED)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_COMPILER g++-7)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

#SRC
set(SIM_SRC random.h random.cpp scheme.h scheme.cpp sde.h sde.cpp simulation.cpp simulation.h montecarlo.cpp montecarlo.h)

#LIB
add_library(SimLib STATIC ${SIM_SRC})

#EXEC
add_executable(sim main.cpp)
target_link_libraries(sim Threads::Threads SimLib)
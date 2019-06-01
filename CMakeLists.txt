cmake_minimum_required(VERSION 3.2)
project(qpp LANGUAGES CXX VERSION 0.7 DESCRIPTION "A c++ quant library")

find_package ( Threads REQUIRED )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER g++-7)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

#SRC
set(QPP_SRC
        qpp.h
        marketmodel.h
        marketmodel.cpp
        payoff.h
        payoff.cpp
        option.h
        option.cpp
        valuation.h
        valuation.cpp
        valuationutils.h
        security.h
        security.cpp)

#LIB
add_library(qpp_lib STATIC ${QPP_SRC})

#EXEC
add_executable(qpp main.cpp)
target_link_libraries(qpp Threads::Threads qpp_lib)
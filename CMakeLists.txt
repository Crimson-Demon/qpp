cmake_minimum_required(VERSION 3.10)
project(qpp)

set(CMAKE_CXX_STANDARD 11)

add_executable(qpp main.cpp security.h security.cpp option.h option.cpp payoff.h payoff.cpp marketmodel.h marketmodel.cpp valuation.h valuationutils.h utils.h)
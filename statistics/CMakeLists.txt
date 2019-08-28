cmake_minimum_required(VERSION 2.8.11)
project(charts_creator)

add_library(statistics statistics.cpp statistics.h cache.cpp cache.h)
target_link_libraries(statistics wiki common)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

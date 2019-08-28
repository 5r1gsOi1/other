cmake_minimum_required(VERSION 2.8.11)
project(charts_creator)

add_library(common
  any.cpp
  any.h
  basic.cpp
  basic.h
  simple_date.cpp
  simple_date.h
  file_index.h
  tests.h)
target_link_libraries(common stdc++fs)
add_subdirectory(tests)


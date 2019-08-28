cmake_minimum_required(VERSION 2.8.11)
project(charts_creator)

add_library(network
  query.h
  query_via_curl.h
  query_via_curl.cpp
  )
#add_subdirectory(tests)
target_link_libraries(network curl)



cmake_minimum_required(VERSION 2.8.11)
project(charts_creator)

add_library(wiki
  wiki.h
  timestamp.h
  query.cpp
  query.h
  parse.cpp
  parse.h
  query_via_network.h
  query_via_network.cpp
  cache.h
  cache_fs.h
  cache_fs.cpp
  page_getter.h
  page_getter.cpp
  )
add_subdirectory(tests)
target_link_libraries(wiki jzon common)



cmake_minimum_required(VERSION 2.8.11)
project(charts_creator)

add_library(rank_afd afd.h afd.cpp)

include_directories("${CMAKE_SOURCE_DIR}/third/re2")


#target_link_libraries(analysis)
# add_subdirectory(tests)



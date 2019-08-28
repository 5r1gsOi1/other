cmake_minimum_required(VERSION 2.8.11)
project(charts_creator)

add_library(analysis analysis.h analysis.cpp)

target_link_libraries(analysis  summaries_charts dibot_charts statistics common chart)
# add_subdirectory(tests)

#detailed_charts



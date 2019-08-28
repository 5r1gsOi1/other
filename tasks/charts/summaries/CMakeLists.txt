cmake_minimum_required(VERSION 2.8.11)
project(charts_creator)

add_library(summaries_charts
  summaries_chart.h
  summaries_chart.cpp
  afi_summaries_chart.h
  afi_summaries_chart.cpp
  )
target_link_libraries(summaries_charts statistics common chart )
#detailed_charts
# add_subdirectory(tests)



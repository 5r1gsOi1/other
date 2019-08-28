cmake_minimum_required(VERSION 2.8.11)
project(charts_creator)

add_library(dibot_charts
  dibot_chart.h
  dibot_chart.cpp
  summaries_chart.h
  summaries_chart.cpp
  )
target_link_libraries(dibot_charts statistics common chart network)
# detailed_charts
# add_subdirectory(tests)



cmake_minimum_required(VERSION 2.8.11)
project(charts_creator)

add_library(detailed_charts
  afd_detailed_chart.h
  afd_detailed_chart.cpp
  afi_detailed_chart.h
  afi_detailed_chart.cpp
  afmo_detailed_chart.h
  afmo_detailed_chart.cpp
  )
target_link_libraries(detailed_charts statistics common chart)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
# add_subdirectory(tests)



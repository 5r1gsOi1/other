cmake_minimum_required(VERSION 2.8.11)
project(charts_creator)

add_library(chart
  chart.h
  chart.cpp
  svg.cpp svg.h
  svg_chart.cpp svg_chart.h
  coordinates_transformer.h coordinates_transformer.cpp
  curves.h curves.cpp

  date_chart/curves.h date_chart/curves.cpp
  date_chart/chart.h date_chart/chart.cpp
  )
add_subdirectory(tests)



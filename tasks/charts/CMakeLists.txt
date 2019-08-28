cmake_minimum_required(VERSION 2.8.11)
project(charts_creator)

add_library(charts charts.h charts.cpp)

#add_subdirectory(detailed)
add_subdirectory(summaries)
add_subdirectory(dibot)

target_link_libraries(charts  summaries_charts dibot_charts statistics common chart)

#detailed_charts

# add_subdirectory(tests)



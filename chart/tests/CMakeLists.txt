cmake_minimum_required(VERSION 2.8.11)
project(charts_creator)

add_executable(test_svg_chart svg.cpp)
target_link_libraries(test_svg_chart chart)
add_test(NAME svg_chart COMMAND test_svg_chart)

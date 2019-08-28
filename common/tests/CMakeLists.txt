cmake_minimum_required(VERSION 2.8.11)
project(charts_creator)

add_executable(test_basic basic.cpp)
target_link_libraries(test_basic common)
add_test(NAME basic COMMAND test_basic)



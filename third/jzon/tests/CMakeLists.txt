cmake_minimum_required(VERSION 2.8.11)
project(charts_creator)

add_executable(test_jzon_decode decode.cpp)
target_link_libraries(test_jzon_decode jzon)
add_test(NAME jzon_decode COMMAND test_jzon_decode)

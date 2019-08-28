cmake_minimum_required(VERSION 2.8.11)
project(charts_creator)

add_executable(test_detailed_afmo test_afmo.cpp)
#target_link_libraries(test_jzon_decode jzon)
add_test(NAME detailed_afmo COMMAND test_detailed_afmo)

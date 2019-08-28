cmake_minimum_required(VERSION 2.8.11)
project(charts_creator)

add_executable(test_statistics statistics.cpp)
target_link_libraries(test_statistics statistics common wiki curl)
add_test(NAME statistics COMMAND test_statistics)

add_executable(test_cache cache.cpp)
target_link_libraries(test_cache statistics common wiki curl)
add_test(NAME cache COMMAND test_cache)

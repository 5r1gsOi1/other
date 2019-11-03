cmake_minimum_required(VERSION 2.8.11)
project(charts_creator)

add_executable(test_wiki_parse parse1.cpp)
target_link_libraries(test_wiki_parse wiki common)
add_test(NAME wiki_parse COMMAND test_wiki_parse)

add_executable(test_wiki_query query.cpp)
target_include_directories(test_wiki_query PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/third/fmt/include>)
target_link_libraries(test_wiki_query PUBLIC wiki common network fmt)
add_test(NAME wiki_query COMMAND test_wiki_query)

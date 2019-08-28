cmake_minimum_required(VERSION 2.8.11)
project(charts_creator)

set(CMAKE_CXX_STANDARD 17)
set(GCC_COMPILE_FLAGS "-g -pg")
add_definitions(${GCC_COMPILE_FLAGS})

include(CTest)

if(BUILD_TESTING)
  enable_testing()
endif()

include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_BINARY_DIR}")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COMPILE_FLAGS}")

add_subdirectory(common)
add_subdirectory(third)
add_subdirectory(wiki)
add_subdirectory(network)
add_subdirectory(parse)
add_subdirectory(chart)
add_subdirectory(statistics)
add_subdirectory(tasks)

add_executable(main main.cpp)
target_link_libraries(main jzon wiki statistics chart charts analysis network rank_afi)




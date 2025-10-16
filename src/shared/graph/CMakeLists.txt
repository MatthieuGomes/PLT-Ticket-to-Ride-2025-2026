cmake_minimum_required(VERSION 3.16)
project(GraphSystem LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(Boost 1.74 REQUIRED COMPONENTS graph)
include_directories(${Boost_INCLUDE_DIRS})

file(GLOB GRAPH_SOURCES "*.cpp")

add_library(graphlib STATIC ${GRAPH_SOURCES})
target_link_libraries(graphlib ${Boost_LIBRARIES})

add_executable(main main.cpp)
target_link_libraries(main graphlib ${Boost_LIBRARIES})

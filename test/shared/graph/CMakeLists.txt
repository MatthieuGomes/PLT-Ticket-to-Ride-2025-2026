cmake_minimum_required(VERSION 3.16)
project(GraphTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.74 REQUIRED COMPONENTS graph unit_test_framework)

set(GRAPH_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../src/shared/graph)
include_directories(${Boost_INCLUDE_DIRS} ${GRAPH_SRC_DIR})

file(GLOB GRAPH_SOURCES
    ${GRAPH_SRC_DIR}/*.cpp
)

# links static library for graph
add_library(graphlib STATIC ${GRAPH_SOURCES})
target_link_libraries(graphlib ${Boost_LIBRARIES})
target_include_directories(graphlib PUBLIC ${GRAPH_SRC_DIR})

# builds test executable
add_executable(test_graph test_Map.cpp)
target_link_libraries(test_graph graphlib ${Boost_LIBRARIES})
target_include_directories(test_graph PRIVATE ${GRAPH_SRC_DIR})

# enable tests
enable_testing()
add_test(NAME GraphUnitTests COMMAND test_graph)

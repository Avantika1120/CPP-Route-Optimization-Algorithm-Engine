cmake_minimum_required(VERSION 3.16)
project(route_optimizer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(route_core
    src/Graph.cpp
    src/Algorithms.cpp
    src/RouteOptimizer.cpp
)
target_include_directories(route_core PUBLIC include)

target_compile_options(route_core PRIVATE -Wall -Wextra -Wpedantic)

add_executable(route_optimizer src/main.cpp)
target_link_libraries(route_optimizer PRIVATE route_core)

add_executable(route_tests tests/test_algorithms.cpp)
target_link_libraries(route_tests PRIVATE route_core)

enable_testing()
add_test(NAME route_tests COMMAND route_tests)

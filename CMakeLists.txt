cmake_minimum_required(VERSION 2.6)
project(Pathfinder)

# The version number
set (Pathfinder_VERSION_MAJOR 1)
set (Pathfinder_VERSION_MINOR 0)

set(PROJECT_BINARY_DIR ./bin)
set(PROJECT_SRC_DIR ./src)

configure_file(
    "${PROJECT_SOURCE_DIR}/config/PathfinderConfig.h.in"
    "${PROJECT_BINARY_DIR}/config/PathfinderConfig.h"
    )
include_directories("${PROJECT_BINARY_DIR}")

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.49.0 COMPONENTS)

include_directories({$Boost_INCLUDE_DIRS})

add_library(Boost src/svg_calculate.cpp)

add_executable(bin/Pathfinder src/main.cpp)
target_link_libraries(bin/Pathfinder ${Boost_LIBRARIES})

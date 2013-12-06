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

## 
#	BOOST
##

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.53.0 COMPONENTS)
include_directories({$Boost_INCLUDE_DIRS})

## 
#	CGAL
##

find_package(CGAL QUIET COMPONENTS Core Qt4)
include( ${CGAL_USE_FILE} )

## 
#	QT
##
set( QT_USE_QTXML    TRUE )
set( QT_USE_QTMAIN   TRUE )
set( QT_USE_QTSCRIPT  TRUE )
set( QT_USE_QTOPENGL  TRUE )

find_package(Qt4 )
include(${QT_USE_FILE})

add_library(Boost src/svg_calculate.cpp)
add_library(CGAL src/main.cpp)
add_library(Qt4 src/main.cpp)

add_executable(bin/Pathfinder src/main.cpp)
target_link_libraries(bin/Pathfinder ${Boost_LIBRARIES})
target_link_libraries(bin/Pathfinder ${QT_LIBRARIES})
target_link_libraries(bin/Pathfinder ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})


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
include_directories("${PROJECT_SRC_DIR}")

# Add C++11 flags for foreach and regexp
#ADD_DEFINITIONS(
    #-std=c++11
#)

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

find_package(CGAL COMPONENTS Core Qt4)
include( ${CGAL_USE_FILE} )

## 
#	QT
##
set( QT_USE_QTXML     TRUE )
set( QT_USE_QTMAIN    TRUE )
set( QT_USE_QTSCRIPT  TRUE )
set( QT_USE_QTOPENGL  TRUE )

include(${QT_USE_FILE})

if( CGAL_Core_FOUND)
    add_definitions(-DCGAL_USE_CORE)
endif()

add_executable(bin/Pathfinder src/main)

add_library(BeachbotLibrary src/polygon_calculate src/view)

target_link_libraries(bin/Pathfinder ${QT_LIBRARIES})
target_link_libraries(bin/Pathfinder ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})

target_link_libraries(bin/Pathfinder BeachbotLibrary)

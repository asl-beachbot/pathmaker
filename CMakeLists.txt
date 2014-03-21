cmake_minimum_required(VERSION 2.6)
project(pathfinder)

# The version number
set (pathfinder_VERSION_MAJOR 1)
set (pathfinder_VERSION_MINOR 0)

set(PROJECT_BINARY_DIR ./bin)
set(PROJECT_SRC_DIR ./src)

configure_file(
    "${PROJECT_SOURCE_DIR}/config/PathfinderConfig.h.in"
    "${PROJECT_BINARY_DIR}/config/PathfinderConfig.h"
)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SRC_DIR}")

# Add C++11 flags for foreach and regexp
ADD_DEFINITIONS(
    #-std=c++11
    -frounding-math
)

## 
#	BOOST
##

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK ON)

find_package(Boost 1.53.0 COMPONENTS)
include_directories({$Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
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

##
#   Eigen
##

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

if( CGAL_Core_FOUND)
    add_definitions(-DCGAL_USE_CORE)
endif()

set(pathfinder_ui ui/pathfinder.ui)
set(pathfinder_qt_src src/view.h)

QT4_WRAP_CPP(pathfinder_HEADERS_MOC ${pathfinder_qt_src})
QT4_WRAP_UI(ui/pathfinder_UI_HEADERS ${pathfinder_ui})

add_executable(bin/pathfinder src/main ui/${pathfinder_UI_HEADERS} ${pathfinder_HEADERS_MOC})

add_library(BeachbotLibrary src/polygon_calculate src/view)

target_link_libraries(bin/pathfinder ${QT_LIBRARIES})
target_link_libraries(bin/pathfinder ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})

target_link_libraries(bin/pathfinder BeachbotLibrary)

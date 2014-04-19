cmake_minimum_required(VERSION 2.6)
project(pathfinder)

# The version number
set (pathfinder_VERSION_MAJOR 1)
set (pathfinder_VERSION_MINOR 0)

set(PROJECT_BINARY_DIR ./bin)
set(PROJECT_SRC_DIR ./src)

set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules/")
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)
include(cotire)
set (COTIRE_MINIMUM_NUMBER_OF_TARGET_SOURCES 1)
configure_file(
    "${PROJECT_SOURCE_DIR}/config/PathfinderConfig.h.in"
    "${PROJECT_BINARY_DIR}/config/PathfinderConfig.h"
)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SRC_DIR}")

# Add C++11 flags for foreach and regexp
# REMOVE O0 slows down execution!!
ADD_DEFINITIONS(
    -std=c++11
    -frounding-math
    -O0
)


set(Python_ADDITIONAL_VERSIONS 3.4)

##
#	BOOST
##

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK ON)

find_package(Boost 1.54.0 COMPONENTS python-py34 filesystem)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

##
#   PYTHON
##
# find_package(PythonInterp 3)
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
link_directories(${PYTHON_LIBRARIES})

##
#	CGAL
##

find_package(CGAL COMPONENTS Core Qt4 PATHS src/include/CGAL)
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


# if(CGAL_Core_FOUND)
#     add_definitions(-DCGAL_USE_CORE)
# endif()

SET(pathfinder_ui ui/pathfinder.ui)
SET(pathfinder_qt_src src/view.h)

QT4_WRAP_CPP(pathfinder_HEADERS_MOC ${pathfinder_qt_src})
QT4_WRAP_UI(ui/pathfinder_UI_HEADERS ${pathfinder_ui})

include_directories(src/include)

add_executable(pathfinder
	src/main.cpp
	src/polygon_calculate.cpp
	src/view.cpp
	${pathfinder_UI_HEADERS}
	${pathfinder_HEADERS_MOC})

add_executable(svg_parser
    src/SVGParserAdapter.cpp
    src/VectorElementTree.h src/include/CGAL_Headers.h
    src/include/tree.h)

target_link_libraries(svg_parser  ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
# add_executable(bin/python_test src/python_wrapper)
# target_link_libraries(bin/python_test ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})

# add_library(BeachbotLibrary )

target_link_libraries(pathfinder ${Boost_LIBRARIES} ${PYTHON_LIBRARIES})
target_link_libraries(pathfinder ${QT_LIBRARIES})
target_link_libraries(pathfinder ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES})

# cotire(pathfinder) #cant cotire because of python vs qt bug!
cotire(svg_parser)

message("THANK YOU FOR CHOOSING PATHMAKER: ") 
message("  CGAL: ${CGAL_USE_FILE}") 
message("  BOOST: ${CGAL_USE_FILE}") 
message("  CGAL: ${CGAL_USE_FILE}") 

# target_link_libraries(bin/pathfinder BeachbotLibrary)

cmake_minimum_required(VERSION 2.6)
project(pathfinder)

option(32BIT "32BIT" OFF)
option(NOGUI "NOGUI" OFF)

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
    -std=c++0x
    -frounding-math
    -O3
    # -H
)

##
#   PYTHON
##

# find_package(PythonInterp 3)
set(Python_ADDITIONAL_VERSIONS 2.7)
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
link_directories(${PYTHON_LIBRARIES})

##
#	BOOST
##

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_ALL_DYN_LINK ON)

find_package(Boost COMPONENTS python-py27 filesystem program_options system)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})


##
#	CGAL
##

if(NOGUI)
    set(COMPS Core)
else(NOGUI)
    set(COMPS Core Qt4)
endif(NOGUI)

find_package(CGAL COMPONENTS ${COMPS} PATHS src/include/CGAL)
include( ${CGAL_USE_FILE} )

##
#	QT
##

if(NOT NOGUI)
    FIND_PACKAGE(Qt4 REQUIRED)
    set( QT_USE_QTXML     TRUE )
    set( QT_USE_QTMAIN    TRUE )
    set( QT_USE_QTSCRIPT  TRUE )
    set( QT_USE_QTOPENGL  TRUE )

    include(${QT_USE_FILE})

    SET(pathfinder_ui ui/pathfinder.ui)
    SET(pathfinder_qt_src src/view.h)

    QT4_WRAP_CPP(pathfinder_HEADERS_MOC ${pathfinder_qt_src})
    QT4_WRAP_UI(ui/pathfinder_UI_HEADERS ${pathfinder_ui})
endif(NOT NOGUI)
##
#   Eigen
##

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

##
#  JSON Write / Parse
##
set(JSONCPP_NAMES "libjsoncpp.so")
find_package(JSONCPP REQUIRED)
include_directories(${JSONCPP_INCLUDE_DIR})

include_directories(src/include)
include_directories(src/include/LKH-2.0.2/SRC src/include/LKH-2.0.2/SRC/INCLUDE)

add_executable(svg_parser
    src/SVGParserAdapter.cpp
    src/PostProcessor.cpp
    src/PreProcessor.cpp
    src/SimpleConnector.cpp
    src/View.cpp
    src/Spiro.cpp
    src/ConnectionSmoother.cpp
    src/TSPConnector.cpp
    src/FillProcedures.cpp
)

if(NOGUI)
    set(COMP_DEFS STANDALONE)
else(NOGUI)
    set(COMP_DEFS WITH_GUI STANDALONE)
endif(NOGUI)


target_link_libraries(svg_parser  ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${QT_LIBRARIES} ${JSONCPP_LIBRARIES})
set_target_properties(svg_parser PROPERTIES COMPILE_DEFINITIONS "${COMP_DEFS}")

## Adding Python Lib
add_library(python/beachbot_pathgen SHARED 
    src/PythonBindings.cpp
    src/SVGParserAdapter.cpp
    src/PreProcessor.cpp
    src/SimpleConnector.cpp
    src/Spiro.cpp
    src/ConnectionSmoother.cpp
    src/FillProcedures.cpp
    src/TSPConnector.cpp
)

if(32BIT)
    set_target_properties(python/beachbot_pathgen PROPERTIES PREFIX "" COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
else(32BIT)
    set_target_properties(python/beachbot_pathgen PROPERTIES PREFIX "")
endif(32BIT)

target_link_libraries(python/beachbot_pathgen ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} ${QT_LIBRARIES} ${JSONCPP_LIBRARIES})

message("THANK YOU FOR CHOOSING PATHMAKER: ") 
message("  CGAL: ${CGAL_USE_FILE}") 
message("  BOOST: ${Boost_LIBRARY_DIRS}") 
message("  COMPILE FLAGS: ${COMP_DEFS}")

# target_link_libraries(bin/pathfinder BeachbotLibrary)

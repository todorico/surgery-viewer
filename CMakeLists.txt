cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(
    test_cgal
    VERSION 0.1.0
    LANGUAGES CXX)

#
# Build Options
#

# set_property(CACHE CMAKE_CXX_FLAGS_COMPILEFAST "-Og")
# set(CMAKE_CXX_FLAGS_COMPILEFAST "-Og" )
include(cmake/BuildSettings.cmake)


# include(cmake/PrecompiledHeaders.cmake)

#
# Dependencies
#

# CGAL PATH
if(NOT DEFINED CGAL_DIR)

	if(EXISTS ${CMAKE_SOURCE_DIR}/CGAL-5.0.1/build)
    	set(CGAL_DIR ${CMAKE_SOURCE_DIR}/CGAL-5.0.1/build) # CGAL shared
	else()
    	set(CGAL_DIR ${CMAKE_SOURCE_DIR}/CGAL-5.0.1) # CGAL header-only
	endif()

endif()

message(STATUS "Using CGAL_DIR at ${CGAL_DIR}")

# required to use basic_viewer

add_definitions(-DCGAL_USE_BASIC_VIEWER -DQT_NO_KEYWORDS)

# CGAL and its components

find_package(CGAL REQUIRED COMPONENTS Qt5)

if(NOT CGAL_FOUND OR NOT CGAL_Qt5_FOUND)
    message(STATUS "This project requires the CGAL library With Qt5 extention, and will not be compiled.")
    return()
endif()

find_package(Boost REQUIRED)

if(NOT Boost_FOUND)
    message(STATUS "This project requires the Boost library, and will not be compiled.")
    return()
endif()

add_subdirectory(src)
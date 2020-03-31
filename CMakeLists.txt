# check_ipo_supported need       : cmake_minimum 3.9.6
# target_precompile_headers need : cmake_minimum 3.16.4
cmake_minimum_required(VERSION 3.16.4)

project(project_name VERSION 0.1.0 LANGUAGES CXX)

#
# Build Options (Possible build type values are cached for ccmake and cmake-gui)
#

# To use clang before cmake .. do :
# export CC=clang-9
# export CXX=clang++-9
# export LD=ld.lld-9

include(cmake/BuildSettings.cmake)
try_enable_ccache(ON)
try_enable_ipo(OFF) # Slowdown linking and give better binaries (Unix only)

# Interface for build options (link this with target to set c++ standard / compile-time options etc...)
add_library(project_build_options INTERFACE)
add_library(project_precompilation_options INTERFACE)
# target_compile_features(project_build_options INTERFACE cxx_std_14) # force use of c++14

# add_definitions("-fdiagnostics-color") # Enable compiler color output

#
# Compiler Warnings
#

include(cmake/CompilerWarnings.cmake)
add_compiler_warnings(project_build_options)

#
# Sanitizers
#

# Address memory and thread are not combinable
include(cmake/Sanitizers.cmake)
enable_sanatizer_address(OFF)
enable_sanatizer_memory(OFF)
enable_sanatizer_thread(OFF)
enable_sanatizer_undefined_behavior(OFF)
try_add_sanitizers(project_build_options) # Must be called last

#
# Static Analyzer Options
#

include(cmake/Cppcheck.cmake)
enable_cppcheck(OFF)

include(cmake/ClangTidy.cmake)
enable_clang_tidy(OFF) # For best practices

# include(cmake/Coverage.cmake)
# enable_coverage(project_build_options ON)

#
# Deployment (Package Manager / Dependencies)
#

# include(cmake/Conan.cmake)
# run_conan()

#
# Documentation
#

include(cmake/Doxygen.cmake)
enable_doxygen(OFF)

#
# Tests
#
# option(BUILD_SHARED_LIBS "Enable compilation of shared libraries" OFF) option(ENABLE_TESTING "Enable Test Builds" OFF)
# option(ENABLE_FUZZING "Enable Test Builds" OFF)
#include(cmake/UnitTest.cmake)
#include(cmake/FuzzTest.cmake)


#
# Dependencies
#

# sudo apt install libboost-dev 'libqt5*-dev' libgmp-dev libmpfr-dev

if(NOT DEFINED CGAL_DIR)

	if(EXISTS ${CMAKE_SOURCE_DIR}/CGAL-5.0.1/Build-RelWithDebInfo)
    	set(CGAL_DIR ${CMAKE_SOURCE_DIR}/CGAL-5.0.1/Build-RelWithDebInfo) # CGAL shared
	else()
    	set(CGAL_DIR ${CMAKE_SOURCE_DIR}/CGAL-5.0.1) # CGAL header-only
    	message(WARNING "Cannot found build folder in CGAL using compilation in header-only mode")
	endif()

endif()

message(STATUS "Using CGAL_DIR at ${CGAL_DIR}")

# Required to use basic_viewer
add_definitions(-DCGAL_USE_BASIC_VIEWER -DQT_NO_KEYWORDS)
# Required for floating operations with CGAL
add_definitions(-frounding-math)
# Add uftrace symbol for debugging
add_definitions(-pg)

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

target_link_libraries(project_build_options INTERFACE CGAL::CGAL CGAL::CGAL_Qt5)

add_subdirectory(src)

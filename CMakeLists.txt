# check_ipo_supported need       : cmake_minimum 3.9.6
# target_precompile_headers need : cmake_minimum 3.16
cmake_minimum_required(VERSION 3.16)

project(project_name VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake) # usefull for configuration

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
target_compile_features(project_build_options INTERFACE cxx_std_17) # force use of c++14


# add_compile_options("/std:c++17")

# add_definitions("-fdiagnostics-color") # Enable compiler color output
# add_definitions(-std=c++17 -lstdc++fs) # Enable compiler color output

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

# sudo apt-get install libboost-dev 'libqt5*-dev' libgmp-dev libmpfr-dev libeigen3-dev # for solver (and libceres-dev for smoother)

# if(NOT DEFINED CGAL_DIR)
# 
# 	if(EXISTS ${CMAKE_SOURCE_DIR}/CGAL-5.0.1/build)
#     	set(CGAL_DIR ${CMAKE_SOURCE_DIR}/CGAL-5.0.1/build) # CGAL shared
# 	else()
#     	set(CGAL_DIR ${CMAKE_SOURCE_DIR}/CGAL-5.0.1) # CGAL header-only
#     	message(WARNING "Cannot found build folder in CGAL using compilation in header-only mode")
# 	endif()
# 
# endif()

# message(STATUS "Using CGAL_DIR at ${CGAL_DIR}")

# Required to use CGAL::Basic_qt_viewer
# -DCGAL_PMP_USE_CERES_SOLVER
# Required for floating operations with CGAL
# add_definitions(-frounding-math)
# Add uftrace symbol for debugging
# add_definitions(-pg)

set(Qt5_DIR     "C:/Qt/5.15.0/mingw81_64/lib/cmake/Qt5")
#set(Qt5Core_DIR "C:/Qt/5.15.0/mingw81_64/lib/cmake/Qt5Core")
#set(QT_QMAKE_EXECUTABLE "C:/Qt/5.15.0/mingw81_64/bin/qmake.exe")


#set(Qt5Core_DIR "C:/Qt/5.15.0/msvc2019_64/lib/cmake/Qt5Core")
#set(Qt5_DIR     "C:/Qt/5.15.0/msvc2019_64/lib/cmake/Qt5")
#set(QT_QMAKE_EXECUTABLE "C:/Qt/5.15.0/msvc2019_64/bin/qmake.exe")

set(GMP_INCLUDE_DIR "C:/Dev/CGAL-5.0.2/auxiliary/gmp/include")

set(MPFR_INCLUDE_DIR "C:/Dev/CGAL-5.0.2/auxiliary/gmp/include")

#set(WITH_CGAL_Qt5 ON)
# set(CMAKE_PREFIX_PATH "C:/Qt/5.15.0/mingw81_64")

# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_add_remote(NAME
				  	bincrafters
				 URL
    				https://api.bintray.com/conan/bincrafters/public-conan)

#

#[[find_program(BASH bash)

if (NOT BASH)
	message(FATAL_ERROR "need bash to build")
else()
	message("BASH path ${BASH}")
endif()]]

# set(BUILD_REQUIRES msys2_installer/latest@bincrafters/stable)

					#qt/5.15.0@bincrafters/stable
# SETTINGS
#					compiler.exception=seh
#					compiler.threads=posix

					#mpfr/4.0.2
					#mpfr/4.0.2@bincrafters/stable 
					#gmp/6.1.2@bincrafters/stable 
					#gmp/6.2.0
					#mpfr/4.0.2@bincrafters/stable 

#conan_config_install(ITEM https://github.com/CGAL/conan-mpfr.git TARGET ${CMAKE_SOURCE_DIR}/lol)
					#mpfr/4.0.2
					#gmp/6.2.0
#              		ENV CONAN_BASH_PATH="C:/'Program Files'/Git/git-bash"

conan_cmake_run(REQUIRES
					assimp/5.0.1
					boost/1.72.0
					docopt.cpp/0.6.2
					eigen/3.3.7
					glm/0.9.9.8
                OPTIONS
                	boost:header_only=True
                BASIC_SETUP
                BUILD missing)
                #PROFILE default
                #PROFILE_AUTO ALL




# if(NOT DEFINED CGAL_DIR)
#     set(CGAL_DIR ${CMAKE_SOURCE_DIR}/lib/CGAL-5.0.2) # CGAL header-only
# endif()

# find_package(CGAL REQUIRED COMPONENTS Qt5)

# if(NOT CGAL_Qt5_FOUND)
#     message(FATAL_ERROR "libcgal with qt5 is required to build this project !")
# endif()

# add_definitions(-DCGAL_USE_BASIC_VIEWER -DQT_NO_KEYWORDS)

# find_package(Qt5 COMPONENTS REQUIRED Core Gui OpenGL Svg Widgets Xml)

# message(STATUS "my lib: ${Qt5Core_LIBRARIES}")

# find_package(Qt5 REQUIRED COMPONENTS OpenGL)

# qtbase5-dev
# Dépend: libqt5opengl5-dev
# Dépend: libqt5svg5-dev
# list(APPEND CMAKE_PREFIX_PATH )
# message(STATUS ${CMAKE_PREFIX_PATH})

include(cmake/ConfigureCGAL.cmake)
include(cmake/ConfigureOpenGL.cmake)
# include(cmake/ConfigureAssimp.cmake)


# target_link_libraries(project_build_options INTERFACE CGAL::CGAL CGAL::CGAL_Qt5 Eigen3::Eigen ${OPENGL_LIBRARIES} ${assimp_LIBRARIES})
# target_link_libraries(project_build_options INTERFACE ${CONAN_LIBS} CGAL::CGAL_Qt5 Qt5::Core Qt5::Widgets Qt5::OpenGL Qt5::Svg ${OPENGL_LIBRARIES})
target_link_libraries(project_build_options INTERFACE ${CONAN_LIBS} CGAL::CGAL_Qt5 ${OPENGL_LIBRARIES})

# OpenGL::OpenGL

add_subdirectory(src)

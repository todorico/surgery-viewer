# check_ipo_supported need       : cmake_minimum 3.9.6
# target_precompile_headers need : cmake_minimum 3.16
cmake_minimum_required(VERSION 3.16)

project(surgery_viewer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

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
target_compile_features(project_build_options INTERFACE cxx_std_17) # force use of c++17

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

# Download automatically, you can also just copy the conan.cmake file

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.15/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

# conan_add_remote(NAME
#             bincrafters
#          URL
#             https://api.bintray.com/conan/bincrafters/public-conan)

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

include(cmake/ConfigureCGAL.cmake)
include(cmake/ConfigureOpenGL.cmake)

target_link_libraries(project_build_options INTERFACE ${CONAN_LIBS} CGAL::CGAL_Qt5 ${OPENGL_LIBRARIES})

add_subdirectory(src)

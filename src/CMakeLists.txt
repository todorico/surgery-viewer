

file(GLOB MAIN_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

# message(STATUS "SOURCES POUR LES EXECUTABLES ${MAIN_SOURCES}")

# add_definitions("-H") # Flag for headers debugging
# target_precompile_headers(project_build_options INTERFACE <iostream> <vector> <string> <map> <utility>)
# add_library(pch cgal_pch.hpp)
# target_precompile_headers(pch INTERFACE "$<$<COMPILE_LANGUAGE:CXX>:cgal_pch.hpp>")

add_library(shared_headers_lib INTERFACE)
target_precompile_headers(shared_headers_lib INTERFACE pch/STD.h pch/CGAL.h docopt/docopt.h)

# Cette library permet precompiler des headers reutilisables pour d'autres cibles
add_library(shared_dependencies_lib OBJECT docopt/docopt.cpp) # create pch lib
target_link_libraries(shared_dependencies_lib PRIVATE project_build_options shared_headers_lib) # Add build options to pchlib

foreach(main_source ${MAIN_SOURCES})

  get_filename_component(main_bin ${main_source} NAME_WE)
  
  add_executable(${main_bin} ${main_source} $<TARGET_OBJECTS:shared_dependencies_lib>)
  target_precompile_headers(${main_bin} REUSE_FROM shared_dependencies_lib)
  target_link_libraries(${main_bin} PRIVATE project_build_options)

endforeach(main_source ${MAIN_SOURCES})

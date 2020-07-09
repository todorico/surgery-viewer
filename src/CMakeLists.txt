


# message(STATUS "SOURCES POUR LES EXECUTABLES ${MAIN_SOURCES}")

# add_definitions("-H") # Flag for headers debugging
# target_precompile_headers(project_build_options INTERFACE <iostream> <vector> <string> <map> <utility>)
# add_library(pch cgal_pch.hpp)
# target_precompile_headers(pch INTERFACE "$<$<COMPILE_LANGUAGE:CXX>:cgal_pch.hpp>")

add_library(shared_headers_lib INTERFACE)
target_precompile_headers(shared_headers_lib INTERFACE pch/STD.h pch/CGAL.h)

file(GLOB MESH_SOURCES ${CMAKE_CURRENT_LIST_DIR}/mesh/*.cpp)

# Cette library permet precompiler des headers reutilisables pour d'autres cibles
add_library(shared_dependencies_lib OBJECT ${MESH_SOURCES}) # create pch lib
target_link_libraries(shared_dependencies_lib PRIVATE project_build_options shared_headers_lib) # Add build options to pchlib

### match

# file(GLOB MATCH_SOURCES match.cpp conversion.cpp data.cpp qglmesh.cpp viewer.cpp)
  
# add_executable(match ${MATCH_SOURCES} $<TARGET_OBJECTS:shared_dependencies_lib>)
# target_precompile_headers(match REUSE_FROM shared_dependencies_lib)
# target_link_libraries(match PRIVATE project_build_options)




# # Cette library permet precompiler des headers reutilisables pour d'autres cibles
# add_library(shared_import_lib OBJECT mesh/conversion.cpp mesh/data.cpp) # create pch lib
# target_link_libraries(shared_import_lib PRIVATE project_build_options shared_headers_lib) # Add build options to pchlib


### prop

# file(GLOB PROP_SOURCES prop.cpp)
  
# add_executable(prop prop.cpp $<TARGET_OBJECTS:shared_dependencies_lib>)
# target_precompile_headers(prop REUSE_FROM shared_dependencies_lib)
# target_link_libraries(prop PRIVATE project_build_options)

### test
  
# add_executable(test ${MATCH_SOURCES} $<TARGET_OBJECTS:shared_dependencies_lib>)
# target_precompile_headers(test REUSE_FROM shared_dependencies_lib)
# target_link_libraries(test PRIVATE project_build_options)

### view

# file(GLOB VIEW_SOURCES view.cpp conversion.cpp data.cpp qglmesh.cpp viewer.cpp)
  
# add_executable(view ${VIEW_SOURCES} $<TARGET_OBJECTS:shared_dependencies_lib>)
# target_precompile_headers(view REUSE_FROM shared_dependencies_lib)
# target_link_libraries(view PRIVATE project_build_options)

### All

file(GLOB MAIN_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

foreach(main_source ${MAIN_SOURCES})

  get_filename_component(main_bin ${main_source} NAME_WE)
  
  add_executable(${main_bin} ${main_source} ${BUILD_SOURCES} $<TARGET_OBJECTS:shared_dependencies_lib>)
  target_precompile_headers(${main_bin} REUSE_FROM shared_dependencies_lib)
  target_link_libraries(${main_bin} PRIVATE project_build_options)

endforeach(main_source ${MAIN_SOURCES})

### Shared binaries dependancies

add_library(shared_headers_lib INTERFACE)
target_compile_features(shared_headers_lib INTERFACE cxx_std_17)
target_precompile_headers(shared_headers_lib INTERFACE pch/STD.h pch/CGAL.h)

file(GLOB MESH_SOURCES ${CMAKE_CURRENT_LIST_DIR}/mesh/*.cpp)

# Cette library permet precompiler des headers reutilisables pour d'autres cibles
add_library(shared_dependencies_lib OBJECT ${MESH_SOURCES}) # create pch lib
target_compile_features(shared_dependencies_lib INTERFACE cxx_std_17)
target_link_libraries(shared_dependencies_lib PRIVATE project_build_options shared_headers_lib) # Add build options to pchlib

# Copy src/shaders/*.glsl to build/bin directory
configure_file(shader/fragment_color_and_texture.frag ../bin/shader/fragment_color_and_texture.frag COPYONLY)
configure_file(shader/fragment_color_only.frag        ../bin/shader/fragment_color_only.frag        COPYONLY)
configure_file(shader/fragment_texture_only.frag      ../bin/shader/fragment_texture_only.frag      COPYONLY)
configure_file(shader/vertex.vert                     ../bin/shader/vertex.vert                     COPYONLY)

### Binaries building 
file(GLOB MAIN_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

foreach(main_source ${MAIN_SOURCES})

  get_filename_component(main_bin ${main_source} NAME_WE)
  
  add_executable(${main_bin} ${main_source} ${BUILD_SOURCES} $<TARGET_OBJECTS:shared_dependencies_lib>)
  target_precompile_headers(${main_bin} REUSE_FROM shared_dependencies_lib)
  target_link_libraries(${main_bin} PRIVATE project_build_options)

endforeach(main_source ${MAIN_SOURCES})

find_package(OpenGL REQUIRED)

if(NOT OPENGL_FOUND)
    message(FATAL_ERROR "An OpenGL Library is required to build this project !")
endif()

# include_directories(${OpenGL_INCLUDE_DIRS})

# link_directories(${OpenGL_LIBRARY_DIRS})

# add_definitions(${OpenGL_DEFINITIONS})

# list(APPEND LIBRARIES ${OPENGL_LIBRARIES})

# #--- Allows 
# if(UNIX AND NOT APPLE)
#     add_definitions(-DGL_GLEXT_PROTOTYPES=1)
# endif()
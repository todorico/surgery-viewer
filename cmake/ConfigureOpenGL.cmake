find_package(OpenGL REQUIRED)

if(NOT OPENGL_FOUND)
    message(FATAL_ERROR "An OpenGL Library is required to build this project !")
endif()

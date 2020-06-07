find_package(assimp REQUIRED)

if(NOT assimp_FOUND)
	message(FATAL_ERROR "libassimp is required to build this project !")
endif()
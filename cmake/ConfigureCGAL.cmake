if(NOT DEFINED CGAL_DIR)
    set(CGAL_DIR ${CMAKE_SOURCE_DIR}/lib/CGAL-5.0.2) # CGAL header-only
endif()

find_package(CGAL REQUIRED COMPONENTS Qt5)

if(NOT CGAL_Qt5_FOUND)
    message(FATAL_ERROR "libcgal with qt5 is required to build this project !")
endif()

add_definitions(-DCGAL_USE_BASIC_VIEWER -DQT_NO_KEYWORDS)
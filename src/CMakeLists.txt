#
# Sources
#

file(GLOB MAIN_SOURCES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

# message(STATUS "SOURCES POUR LES EXECUTABLES ${MAIN_SOURCES}")

foreach(main_source ${MAIN_SOURCES})
    get_filename_component(main_bin ${main_source} NAME_WE)
    add_executable(${main_bin} ${main_source})
    # target_precompiled_header(${main_bin} pch.hpp FORCE_INCLUDE)
    # add_to_cached_list(CGAL_EXECUTABLE_TARGETS ${main_bin})
    target_link_libraries(${main_bin} CGAL::CGAL CGAL::CGAL_Qt5)
endforeach(main_source ${MAIN_SOURCES})

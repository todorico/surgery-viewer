function(enable_coverage project_name toggle)

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

        option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" ${toggle})

        if(ENABLE_COVERAGE)

            target_compile_options(${project_name} INTERFACE --coverage -O0 -g)
            target_link_libraries(${project_name} INTERFACE --coverage)
        
        endif()
    
    else()

        message(SEND_ERROR "Coverage only supported in gcc / clang")

    endif()

endfunction()

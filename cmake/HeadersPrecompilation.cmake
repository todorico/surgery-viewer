macro(enable_headers_precompilation toggle)
    option(ENABLE_PCH "Enable Headers Precompilation" ${toggle})
endmacro()

function(set_project_precompiled_headers project_name pch_list)
    # Very basic PCH example
    if(ENABLE_PCH)
        # This sets a global PCH parameter, each project will build its own PCH, which is a good idea if any #define's
        # change target_precompile_headers(project_name INTERFACE <vector> <string> <map> <utility>)
        target_precompile_headers(${project_name} INTERFACE ${pch_list})
    endif()

endfunction()

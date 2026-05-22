# Shared warning configuration applied to first-party targets only (never to
# third-party Conan packages). CI sets VR_WARNINGS_AS_ERRORS=ON to fail on drift;
# local builds keep warnings as warnings so a work-in-progress still compiles.

option(VR_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

function(vr_set_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
        if(VR_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE /WX)
        endif()
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
        if(VR_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE -Werror)
        endif()
    endif()
endfunction()

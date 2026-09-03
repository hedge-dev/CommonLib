#
# Merges a static library into another.
#
function(MergeStaticLibrary DST SRC)
    if(WIN32)
        find_program(LIB lib REQUIRED)
        add_custom_command(TARGET ${DST} POST_BUILD
            COMMAND ${LIB} /NOLOGO /OUT:$<TARGET_FILE:${DST}> $<TARGET_FILE:${DST}> "${SRC}"
            COMMENT "Merging static library ${SRC} into ${DST}..."
        )
    else()
        message(STATUS "Merging libraries is not implemented for the target operating system.")
    endif()
endfunction()

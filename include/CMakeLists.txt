include("../toolchains/StaticLibrary.cmake")

list(APPEND CMNLIB_SOURCES "CommonLib.cpp")

add_library(CommonLib STATIC ${CMNLIB_SOURCES})

target_include_directories(CommonLib PRIVATE
    ${CMNLIB_ROOT}
    ${CMNLIB_INCLUDES}
)

if(WIN32)
    if(TARGET Detours)
        add_dependencies(CommonLib Detours)
    endif()
endif()

foreach(CMNLIB_LIB ${CMNLIB_LIBS})
    MergeStaticLibrary(CommonLib ${CMNLIB_LIB})
endforeach()

set(CMNLIB_TARGET_DIR "${CMNLIB_ROOT}/lib/${CMAKE_SYSTEM_NAME}/${CMNLIB_TARGET_ARCH}/${CMAKE_BUILD_TYPE}")

add_custom_command(TARGET CommonLib POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMNLIB_TARGET_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:CommonLib> "${CMNLIB_TARGET_DIR}"
)

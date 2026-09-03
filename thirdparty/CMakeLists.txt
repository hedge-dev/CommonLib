include(ExternalProject)

if(WIN32)
    include("../toolchains/MSVC.cmake")
    
    GetDevCmd(DEVCMD_PATH DEVCMD_ARCH)

    if(NOT (DEVCMD_PATH AND DEVCMD_ARCH))
        message(STATUS "Failed to acquire Visual Studio development environment.")
    endif()

    if(CMNLIB_TARGET_ARCH STREQUAL "x86" OR CMNLIB_TARGET_ARCH STREQUAL "x64" OR CMNLIB_TARGET_ARCH STREQUAL "ARM64")
        find_program(NMAKE nmake REQUIRED)

        set(DETOURS_BUILD_ARGS)
        set(DETOURS_DIR "${CMNLIB_THIRDPARTY_ROOT}/Detours")
        set(DETOURS_LIB "${DETOURS_DIR}/")
    
        if(CMNLIB_TARGET_ARCH STREQUAL "x86")
            list(APPEND DETOURS_BUILD_ARGS "PROCESSOR_ARCHITECTURE=X86")
            string(APPEND DETOURS_LIB "lib.X86")
        elseif (CMNLIB_TARGET_ARCH STREQUAL "x64")
            list(APPEND DETOURS_BUILD_ARGS "PROCESSOR_ARCHITECTURE=AMD64")
            string(APPEND DETOURS_LIB "lib.X64")
        endif()

        string(APPEND DETOURS_LIB "/detours.lib")

        list(APPEND CMNLIB_LIBS ${DETOURS_LIB})
        list(APPEND CMNLIB_INCLUDES "${DETOURS_DIR}/include")

        if(CMNLIB_IS_DEBUG)
            list(APPEND DETOURS_BUILD_ARGS "DETOURS_DEBUG=1")
        endif()

        if(DEVCMD_PATH AND DEVCMD_ARCH)
            ExternalProject_Add(Detours
                PREFIX            "${DETOURS_DIR}"
                SOURCE_DIR        "${DETOURS_DIR}/src"
                CONFIGURE_COMMAND ""
                INSTALL_COMMAND   ""
                BUILD_IN_SOURCE   1
                BUILD_COMMAND     CALL ${DEVCMD_PATH} ${DEVCMD_ARCH} && ${NMAKE} /NOLOGO ${DETOURS_BUILD_ARGS}
                BUILD_BYPRODUCTS  ${DETOURS_LIB}
            )
        else()
            message(STATUS "Detours will not be built.")
        endif()
    else()
        message(STATUS "Detours does not support ${CMNLIB_TARGET_ARCH}.")
    endif()
endif()

set(CMNLIB_LIBS ${CMNLIB_LIBS} PARENT_SCOPE)
set(CMNLIB_INCLUDES ${CMNLIB_INCLUDES} PARENT_SCOPE)
set(CMNLIB_SOURCES ${CMNLIB_SOURCES} PARENT_SCOPE)

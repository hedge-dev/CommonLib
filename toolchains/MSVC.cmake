include("${CMAKE_CURRENT_LIST_DIR}/Processor.cmake")

#
# Gets the path and architecture required to invoke Visual Studio Developer Command Prompt.
#
function(GetDevCmd PATH ARCH)
    GetHostProcessorArchitecture(HOST_ARCH)
    GetTargetProcessorArchitecture(CMNLIB_TARGET_ARCH)

    if(NOT (CMNLIB_TARGET_ARCH STREQUAL "x86" OR CMNLIB_TARGET_ARCH STREQUAL "x64" OR CMNLIB_TARGET_ARCH STREQUAL "ARM64"))
        return()
    endif()

    if(HOST_ARCH STREQUAL CMNLIB_TARGET_ARCH)
        set(${ARCH} "${HOST_ARCH}" PARENT_SCOPE)
    else()
        set(${ARCH} "${HOST_ARCH}_${CMNLIB_TARGET_ARCH}" PARENT_SCOPE)
    endif()
    
    set(${PATH} "${VS_INSTALLATION_PATH}\\VC\\Auxiliary\\Build\\vcvarsall.bat" PARENT_SCOPE)
endfunction()

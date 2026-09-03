# Reference: https://stackoverflow.com/questions/45125516/possible-values-for-uname-m

#
# Gets the architecture with common names.
# Returns x86, x64, ARM and ARM64 exclusively for those platforms, otherwise it returns the full name.
#
function (GetProcessorArchitecture ARCH RESULT)
    string(TOLOWER ${ARCH} ARCH_LOWER)

    if(ARCH_LOWER MATCHES "^(x86|i.86)")
        set(ARCH "x86")
    elseif(ARCH_LOWER MATCHES "^(x64|x86_64|amd64)")
        set(ARCH "x64")
    elseif(ARCH_LOWER MATCHES "^(arm$|armv.)")
        set(ARCH "ARM")
    elseif(ARCH_LOWER MATCHES "^(arm64|aarch64(_be)?)")
        set(ARCH "ARM64")
    endif()

    set(${RESULT} ${ARCH} PARENT_SCOPE)
endfunction()

#
# Gets the endianness of the architecture.
# Returns "L" and "B" for little-endian and big-endian respectively.
#
function(GetProcessorEndianness ARCH RESULT)
    string(TOLOWER ${ARCH} ARCH_LOWER)

    if(ARCH_LOWER MATCHES "^(aarch64_be|armv.b|ppc(64)?|.*risc(64)?|s390(x)?|sparc(64)?)" AND NOT
        ARCH_LOWER MATCHES "(l|le)$")
        set(ENDIANNESS "B")
    else()
        set(ENDIANNESS "L")
    endif()

    set(${RESULT} ${ENDIANNESS} PARENT_SCOPE)
endfunction()

#
# Gets the host architecture with common names.
# Returns x86, x64, ARM and ARM64 exclusively for those platforms, otherwise it returns the full name.
#
function (GetHostProcessorArchitecture RESULT)
    GetProcessorArchitecture(${CMAKE_HOST_SYSTEM_PROCESSOR} ARCH)
    set(${RESULT} ${ARCH} PARENT_SCOPE)
endfunction()

#
# Gets the endianness of the host architecture.
# Returns "L" and "B" for little-endian and big-endian respectively.
#
function(GetHostProcessorEndianness RESULT)
    GetProcessorEndianness(${CMAKE_HOST_SYSTEM_PROCESSOR} ENDIANNESS)
    set(${RESULT} ${ENDIANNESS} PARENT_SCOPE)
endfunction()

#
# Gets the target architecture with common names.
# Returns x86, x64, ARM and ARM64 exclusively for those platforms, otherwise it returns the full name.
#
function (GetTargetProcessorArchitecture RESULT)
    GetProcessorArchitecture(${CMAKE_SYSTEM_PROCESSOR} ARCH)
    set(${RESULT} ${ARCH} PARENT_SCOPE)
endfunction()

#
# Gets the endianness of the target architecture.
# Returns "L" and "B" for little-endian and big-endian respectively.
#
function(GetTargetProcessorEndianness RESULT)
    GetProcessorEndianness(${CMAKE_SYSTEM_PROCESSOR} ENDIANNESS)
    set(${RESULT} ${ENDIANNESS} PARENT_SCOPE)
endfunction()

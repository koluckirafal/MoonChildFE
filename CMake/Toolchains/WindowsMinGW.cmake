set(CMAKE_SYSTEM_NAME Windows)

if(NOT DEFINED MOONCHILD_TARGET_ARCH)
    set(MOONCHILD_TARGET_ARCH "WindowsX64")
endif()

# Windows x64
if(MOONCHILD_TARGET_ARCH STREQUAL "WindowsX64")
    set(CMAKE_SYSTEM_PROCESSOR AMD64)

# Windows x86
elseif(MOONCHILD_TARGET_ARCH STREQUAL "WindowsX86")
    set(CMAKE_SYSTEM_PROCESSOR X86)

else()
    message(FATAL_ERROR "Unsupported Windows target architecture: ${MOONCHILD_TARGET_ARCH}")
endif()

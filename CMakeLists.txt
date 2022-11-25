cmake_minimum_required(VERSION 3.10)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)
project(medusaos LANGUAGES C ASM-ATT)

unset(CMAKE_SYSROOT)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/sysroot)
set(CMAKE_SYSROOT ${CMAKE_BINARY_DIR}/sysroot)
set(CMAKE_C_STANDARD 11)

ENABLE_LANGUAGE(ASM-ATT)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})

if(NOT ARCH)
    message(FATAL_ERROR "Target architecture (ARCH) is not defined. Please, choose one of: i386")
endif()

set_property(CACHE ARCH PROPERTY STRINGS "i386")

if(ARCH STREQUAL "i386")
SET(FIRST_PART_HOST "i686")
endif()

#set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/kernel/arch/${ARCH}/linker.ld")
#set(CMAKE_C_LINK_EXECUTABLE ${FIRST_PART_HOST}-elf-ld)

add_custom_target(run
    qemu-system-${ARCH} -boot order=d -drive file=../disk.img,format=raw,media=disk,if=ide -drive file=../kernel.iso,media=cdrom,readonly=on -serial file:logs.txt
)

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/libc/include)
include_directories(${CMAKE_SOURCE_DIR}/kernel/include)

#set_target_properties(kernel.kernel PROPERTIES LINKER_LANGUAGE C)
#set_target_properties(kernel.kernel PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

add_subdirectory(kernel)
add_subdirectory(libc)

install(DIRECTORY include DESTINATION ${CMAKE_INSTALL_PREFIX}/usr)
CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)
project(medusaos LANGUAGES C ASM-ATT)

ENABLE_LANGUAGE(ASM-ATT)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})

if(NOT ARCH)
    message(FATAL_ERROR "Target architecture (ARCH) is not defined. Please, choose one of: i386")
endif()

set_property(CACHE ARCH PROPERTY STRINGS "i386")

if(ARCH STREQUAL "i386")
SET(FIRST_PART_HOST "i686")
endif()

set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/kernel/arch/${ARCH}/linker.ld")
set(CMAKE_C_LINK_EXECUTABLE ${FIRST_PART_HOST}-elf-ld)

add_custom_target(run
    qemu-system-${ARCH} -boot order=d -drive file=disk.img,format=raw,media=disk,if=ide -drive file=kernel.iso,media=cdrom,readonly=on -serial file:logs.txt
)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libc/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/kernel/include)
add_executable(kernel.kernel dummy.c kernel/arch/${ARCH}/boot.S)

target_link_libraries(kernel.kernel kernel libc)
#set_target_properties(kernel.kernel PROPERTIES LINKER_LANGUAGE C)
set_target_properties(kernel.kernel PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})
ADD_SUBDIRECTORY(kernel)
ADD_SUBDIRECTORY(libc)
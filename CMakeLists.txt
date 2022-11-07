CMAKE_MINIMUM_REQUIRED(VERSION 3.2)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(CMAKE_TOOLCHAIN_FILE toolchain.cmake)
project(MEDUSAOS LANGUAGES C)

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


add_executable(kernel.kernel dummy.c kernel/arch/${ARCH}/boot.S)
set_target_properties(kernel.kernel PROPERTIES LINKER_LANGUAGE C)

target_link_libraries(kernel.kernel kernel libc)
set_target_properties(kernel.kernel PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

ADD_SUBDIRECTORY(kernel)
ADD_SUBDIRECTORY(libc)
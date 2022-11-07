CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
project(MEDUSAOS)
ADD_SUBDIRECTORY(libc)
ADD_SUBDIRECTORY(kernel)


if(NOT ARCH)
    message(FATAL_ERROR "Target architecture (ARCH) is not defined. Please, choose one of: i386")
endif()

set_property(CACHE ARCH PROPERTY STRINGS "i386")

if(ARCH STREQUAL "i386")
set(CMAKE_C_COMPILER   i686-elf-gcc)
endif()



add_custom_target(run
    qemu-system-${ARCH} -boot order=d -drive file=disk.img,format=raw,media=disk,if=ide -drive file=kernel.iso,media=cdrom,readonly=on -serial file:logs.txt
)
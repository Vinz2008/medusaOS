SET(FIRST_PART_HOST "i686")

if(ARCH STREQUAL "i386")
SET(FIRST_PART_HOST "i686")
endif()

SET(HOST "${FIRST_PART_HOST}-elf")


set(CMAKE_AR                    "${HOST}-ar")
set(CMAKE_ASM_COMPILER          "${HOST}-gcc")
set(CMAKE_C_COMPILER            "${HOST}-gcc")
set(CMAKE_CXX_COMPILER          "${HOST}-g++")
set(CMAKE_LINKER                "${HOST}-ld")
set(CMAKE_OBJCOPY               "${HOST}-objcopy")
set(CMAKE_RANLIB               "${HOST}-ranlib")
set(CMAKE_SIZE                  "${HOST}-size")
set(CMAKE_STRIP                 "${HOST}-strip")
set(TOOLCHAIN_PREFIX ${HOST})

set(CMAKE_SYSROOT "${CMAKE_SOURCE_DIR}/sysroot")
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER_WORKS 1)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM     NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY     ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE     ONLY)
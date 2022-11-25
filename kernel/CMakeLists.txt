include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/libc/include)


#set(CMAKE_C_COMPILER "${CMAKE_C_COMPILER} --sysroot=${SYSROOT}")
#set(CMAKE_C_COMPILER "${CMAKE_C_COMPILER} -isystem=${INCLUDEDIR}")

set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g -ffreestanding -Wall -Wextra")
set(CMAKE_CXX_FLAGS "{CMAKE_CXX_FLAGS} -D__is_kernel -Iinclude")
link_libraries( "-nostdlib -lgcc")

set(KERNEL_BIN "kernel")

file(GLOB KERNEL_C_ARCH_MODULES "arch/${ARCH}/*.c")
file(GLOB KERNEL_ASM_ARCH_MODULES "arch/${ARCH}/*.S")
list(REMOVE_ITEM KERNEL_ASM_ARCH_MODULES "arch/${ARCH}/boot.S")
file(GLOB KERNEL_C_MODULES "kernel/*.c")

list(APPEND KERNEL_SOURCES
    ${KERNEL_C_ARCH_MODULES}
    ${KERNEL_C_MODULES}
    ${KERNEL_ASM_ARCH_MODULES}
)


add_executable(kernel.kernel ${KERNEL_SOURCES})

set(CMAKE_C_LINK_FLAGS " \
    -T${CMAKE_CURRENT_SOURCE_DIR}/arch/${ARCH}/linker.ld")
message(${CMAKE_C_LINK_FLAGS})
#set_target_properties(${KERNEL_BIN} PROPERTIES LINK_FLAGS "-n -T${CMAKE_SOURCE_DIR}/arch/${ARCH}/linker.ld")

#ADD_CUSTOM_COMMAND (TARGET ${KERNEL_BIN}
#	POST_BUILD
#	COMMAND "${CMAKE_C_COMPILER} -T${CMAKE_SOURCE_DIR}/arch/${ARCH}/linker.ld -o ${KERNEL_BIN} ${CMAKE_BINARY_DIR}/libkernel.img.a")

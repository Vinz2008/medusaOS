# Declare constants for the multiboot header.

.set TAG_END,          0
.set TAG_FRAMEBUFFER,  5
.set TAG_REQUIRED,     0
.set TAG_OPTIONAL,     1



.set MAGIC, 0xE85250D6      # 'magic number' lets bootloader find the header
.set ARCH, 0
.set HEADER_LEN,       (multiboot_end - multiboot_start)
.set CHECKSUM,         -(MAGIC + ARCH + HEADER_LEN) # checksum of above, to prove we are multiboot
.set KERNEL_VIRTUAL_BASE, 0xC0000000 

.set KERNEL_VIRTUAL_BASE, 0xC0000000
.set KERNEL_PAGE_NUMBER,  (KERNEL_VIRTUAL_BASE >> 22)


# Declare a header as in the Multiboot Standard.
.section .multiboot
multiboot_start:
	.align 8
	.long MAGIC
	.long ARCH
	.long HEADER_LEN
	.long CHECKSUM
	# Graphics tag
    .align 8
    .short TAG_FRAMEBUFFER
    .short TAG_REQUIRED
    .long 20
    .long 1024
    .long 768
    .long 32

    # End tag
    .align 8
    .short TAG_END
    .short TAG_REQUIRED
    .long 8
multiboot_end:

.section .bootstrap_stack, "aw", @nobits
stack_bottom:
    .skip 16384 # 16 KiB
stack_top:

.section .data


.global kernel_directory
kernel_directory:
    .align 0x1000
    .long 0x00000083 # 4 MiB pages
    .long 0x00400083
    .long 0x00800083
    .long 0x00C00083
    .fill (KERNEL_PAGE_NUMBER - 4), 4, 0
    .long 0x00000083
    .fill (1024 - KERNEL_PAGE_NUMBER - 1), 4, 0

# The kernel entry point.
.section .text
.global _start
.type _start, @function
_start:
	mov $(kernel_directory - KERNEL_VIRTUAL_BASE), %ecx
	mov %ecx, %cr3
	mov %cr4, %ecx
    or $0x00000010, %ecx
    mov %ecx, %cr4

    mov %cr0, %ecx
    or $0x80000000, %ecx
    mov %ecx, %cr0

    lea _start_higher_half, %ecx
    jmp *%ecx


_start_higher_half:
	movl $stack_top, %esp
	push %eax
	push %ebx

	# Call the global constructors.
	call _init

	# Transfer control to the main kernel.
	call kernel_main

	# Hang if kernel_main unexpectedly returns.
	cli
1:	hlt
	jmp 1b
.size _start, . - _start

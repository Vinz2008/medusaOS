#include <stdio.h>

#if defined(__is_libk)
#include <kernel/tty.h>
#include <kernel/config.h>
#include <kernel/gui.h>
#endif

int putchar(int ic) {
#if defined(__is_libk)
	char c = (char) ic;
#ifdef GUI_MODE
	//draw_string(&c, 100, 250, 0xFFFFFF);
#else
	terminal_write(&c, sizeof(c));
#endif
#else
	// TODO: Implement stdio and the write system call.
#endif
	return ic;
}

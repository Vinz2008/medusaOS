#include <kernel/fb.h>
#include <kernel/kmalloc.h>
#include <kernel/paging.h>
#include <multiboot2.h>
#include <stdio.h>

static fb_t fb;

void init_fb(struct multiboot_tag_framebuffer* tagfb) {
  fb.width = tagfb->common.framebuffer_width;
  fb.height = tagfb->common.framebuffer_height;
  fb.pitch = tagfb->common.framebuffer_pitch;
  fb.bpp = tagfb->common.framebuffer_bpp;
  if (fb.bpp != 32) {
    log(LOG_SERIAL, false, "Unsupported bit depth %d\n", fb.bpp);
  }
  uintptr_t address = (uintptr_t)tagfb->common.framebuffer_addr;
  log(LOG_SERIAL, false, "framebuffer address : %p\n", address);

  // Remap our framebuffer
  uint32_t size = fb.height * fb.pitch;
  log(LOG_SERIAL, false, "test init fb\n");
  uintptr_t buff = (uintptr_t)kamalloc(size, 0x1000);

  for (uint32_t i = 0; i < size / 0x1000; i++) {
    page_t* p = paging_get_page(buff + 0x1000 * i, false, 0);
    *p = (address + 0x1000 * i) | PAGE_PRESENT | PAGE_RW;
  }

  fb.address = buff;
}

fb_t fb_get_info() { return fb; }
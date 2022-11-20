#include <types.h>

#define MB2_MAGIC 0x36D76289
#define MB2_TAG_END 0
#define MB2_TAG_CMDLINE 1
#define MB2_TAG_MODULE 3
#define MB2_TAG_MEM 4
#define MB2_TAG_MMAP 6
#define MB2_TAG_VBE 7
#define MB2_TAG_FB 8
#define MB2_TAG_APM 10
#define MB2_TAG_RSDP1 14
#define MB2_TAG_RSDP2 15

#define MB2_MMAP_AVAIL 1

typedef struct mb2_tag_t {
    uint32_t type;
    uint32_t size;
} mb2_tag_t __attribute__((packed));

typedef struct mb2_tag_module_t {
    mb2_tag_t header;
    uintptr_t mod_start;
    uintptr_t mod_end;
    unsigned char name[];
} mb2_tag_module_t __attribute__((packed));

typedef struct mb2_mmap_entry_t {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t reserved;
} mb2_mmap_entry_t __attribute__((packed));

typedef struct mb2_tag_mmap_t {
    mb2_tag_t header;
    uint32_t entry_size;
    uint32_t entry_version;
    mb2_mmap_entry_t entries[];
} mb2_tag_mmap_t __attribute__((packed));

typedef struct mb2_t {
    uint32_t total_size;
    uint32_t reserved;
    /* Not an array: just points to the first tag */
    mb2_tag_t tags[];
} mb2_t __attribute__((packed));

mb2_tag_t* mb2_find_tag(mb2_t* boot, uint32_t tag_type);
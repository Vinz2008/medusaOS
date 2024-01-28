#include <kernel/elf.h>
#include <kernel/kmalloc.h>
#include <kernel/paging.h>
#include <kernel/vfs.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// TODO : work on this/remove this

static inline Elf32_Shdr* elf_sheader(Elf32_Ehdr* header) {
  return (Elf32_Shdr*)((int)header + header->e_shoff);
}

static inline Elf32_Shdr* elf_section(Elf32_Ehdr* header, int idx) {
  return &elf_sheader(header)[idx];
}

static inline char* elf_str_table(Elf32_Ehdr* header) {
  if (header->e_shstrndx == SHN_UNDEF) {
    return NULL;
  }
  return (char*)header + elf_section(header, header->e_shstrndx)->sh_offset;
}

static inline char* elf_lookup_string(Elf32_Ehdr* header, int offset) {
  char* strtab = elf_str_table(header);
  if (strtab == NULL) {
    return NULL;
  }
  return strtab + offset;
}

bool elf_check_file(Elf32_Ehdr* header) {
  if (!header) {
    return false;
  }
  if (header->e_ident[EI_MAG0] != ELFMAG0) {
    log(LOG_SERIAL, false, "ERROR : ELF Header EI_MAG0 incorrect.\n");
    return false;
  }
  if (header->e_ident[EI_MAG1] != ELFMAG1) {
    log(LOG_SERIAL, false, "ERROR : ELF Header EI_MAG1 incorrect.\n");
    return false;
  }
  if (header->e_ident[EI_MAG2] != ELFMAG2) {
    log(LOG_SERIAL, false, "ERROR : ELF Header EI_MAG2 incorrect.\n");
    return false;
  }
  if (header->e_ident[EI_MAG3] != ELFMAG3) {
    log(LOG_SERIAL, false, "ERROR : ELF Header EI_MAG3 incorrect.\n");
    return false;
  }
  return true;
}

bool elf_check_supported(Elf32_Ehdr* header) {
  if (!elf_check_file(header)) {
    log(LOG_SERIAL, false, "ERROR : Invalid ELF File.\n");
    return false;
  }
  if (header->e_ident[EI_CLASS] != ELFCLASS32) {
    log(LOG_SERIAL, false, "ERROR : Unsupported ELF File Class.\n");
    return false;
  }
  if (header->e_ident[EI_DATA] != ELFDATA2LSB) {
    log(LOG_SERIAL, false, "ERROR : Unsupported ELF File byte order.\n");
    return false;
  }
  if (header->e_machine != EM_386) {
    log(LOG_SERIAL, false, "ERROR : Unsupported ELF File target.\n");
    return false;
  }
  if (header->e_ident[EI_VERSION] != EV_CURRENT) {
    log(LOG_SERIAL, false, "ERROR : Unsupported ELF File version.\n");
    return false;
  }
  if (header->e_type != ET_REL && header->e_type != ET_EXEC) {
    log(LOG_SERIAL, false, "ERROR : Unsupported ELF File type.\n");
    return false;
  }
  return true;
}

void* elf_lookup_symbol(const char* name) {
  return NULL; // minimal implementation
}

static int elf_get_symval(Elf32_Ehdr* header, int table, uint8_t idx) {
  if (table == SHN_UNDEF || idx == SHN_UNDEF) {
    return 0;
  }
  Elf32_Shdr* symtab = elf_section(header, table);
  uint32_t symtab_entries = symtab->sh_size / symtab->sh_entsize;
  if (idx >= symtab_entries) {
    log(LOG_SERIAL, false, "Symbol Index out of Range (%d:%u).\n", table, idx);
    return ELF_RELOC_ERR;
  }
  int symaddr = (int)header + symtab->sh_offset;
  Elf32_Sym* symbol = &((Elf32_Sym*)symaddr)[idx];
  if (symbol->st_shndx == SHN_UNDEF) {
    // External symbol, lookup value
    Elf32_Shdr* strtab = elf_section(header, symtab->sh_link);
    const char* name =
        (const char*)header + strtab->sh_offset + symbol->st_name;

    extern void* elf_lookup_symbol(const char* name);
    void* target = elf_lookup_symbol(name);

    if (target == NULL) {
      // Extern symbol not found
      if (ELF32_ST_BIND(symbol->st_info) & STB_WEAK) {
        // Weak symbol initialized as 0
        return 0;
      } else {
        log(LOG_SERIAL, false, "Undefined External Symbol : %s.\n", name);
        return ELF_RELOC_ERR;
      }
    } else {
      return (int)target;
    }

  } else if (symbol->st_shndx == SHN_ABS) {
    // Absolute symbol
    return symbol->st_value;
  } else {
    // Internally defined symbol
    Elf32_Shdr* target = elf_section(header, symbol->st_shndx);
    return (int)header + symbol->st_value + target->sh_offset;
  }
}

static int elf_load_stage1(Elf32_Ehdr* header) {
  Elf32_Shdr* shdr = elf_sheader(header);
  unsigned int i;
  // Iterate over section headers
  for (i = 0; i < header->e_shnum; i++) {
    Elf32_Shdr* section = &shdr[i];
    // If the section isn't present in the file
    if (section->sh_type == SHT_NOBITS) {
      // Skip if it the section is empty
      if (!section->sh_size) {
        continue;
      }
      // If the section should appear in memory
      if (section->sh_flags & SHF_ALLOC) {
        // Allocate and zero some memory
        void* mem = kmalloc(section->sh_size);
        memset(mem, 0, section->sh_size);
        // Assign the memory offset to the section offset
        section->sh_offset = (int)mem - (int)header;
        log(LOG_SERIAL, false, "Allocated memory for a section (%ld).\n",
            section->sh_size);
      }
    }
  }
  return 0;
}

static int elf_load_stage2(Elf32_Ehdr* header) {
  Elf32_Shdr* shdr = elf_sheader(header);

  unsigned int i, idx;
  // Iterate over section headers
  for (i = 0; i < header->e_shnum; i++) {
    Elf32_Shdr* section = &shdr[i];
    // If this is a relocation section
    if (section->sh_type == SHT_REL) {
      // Process each entry in the table
      for (idx = 0; idx < section->sh_size / section->sh_entsize; idx++) {
        Elf32_Rel* reltab =
            &((Elf32_Rel*)((int)header + section->sh_offset))[idx];
        int result = elf_do_reloc(header, reltab, section);
        // On error, display a message and return
        if (result == ELF_RELOC_ERR) {
          log(LOG_SERIAL, false, "Failed to relocate symbol.\n");
          return ELF_RELOC_ERR;
        }
      }
    }
  }
  return 0;
}

static inline void* elf_load_rel(Elf32_Ehdr* header) {
  int result;
  result = elf_load_stage1(header);
  if (result == ELF_RELOC_ERR) {
    log(LOG_SERIAL, false, "Unable to load ELF file.\n");
    return NULL;
  }
  result = elf_load_stage2(header);
  if (result == ELF_RELOC_ERR) {
    log(LOG_SERIAL, false, "Unable to load ELF file.\n");
    return NULL;
  }
  // TODO : Parse the program header (if present)
  return (void*)header->e_entry;
}

int elf_do_reloc(Elf32_Ehdr* header, Elf32_Rel* rel, Elf32_Shdr* reltab) {
  Elf32_Shdr* target = elf_section(header, reltab->sh_info);

  int addr = (int)header + target->sh_offset;
  int* ref = (int*)(addr + rel->r_offset);
  // Symbol value
  int symval = 0;
  if (ELF32_R_SYM(rel->r_info) != SHN_UNDEF) {
    symval = elf_get_symval(header, reltab->sh_link, ELF32_R_SYM(rel->r_info));
    if (symval == ELF_RELOC_ERR) {
      return ELF_RELOC_ERR;
    }
  }
  // Relocate based on type
  switch (ELF32_R_TYPE(rel->r_info)) {
  case R_386_NONE:
    // No relocation
    break;
  case R_386_32:
    // Symbol + Offset
    *ref = DO_386_32(symval, *ref);
    break;
  case R_386_PC32:
    // Symbol + Offset - Section Offset
    *ref = DO_386_PC32(symval, *ref, (int)ref);
    break;
  default:
    // Relocation type not supported, display error and return
    log(LOG_SERIAL, false, "Unsupported Relocation Type (%d).\n",
        ELF32_R_TYPE(rel->r_info));
    return ELF_RELOC_ERR;
  }
  return symval;
}

void elf_start_executable(void* file) {}

char* current_process_name =
    ""; // TODO : INTEGRATE THIS WITH SCHEDULING AND PROCESS SYSTEM
uintptr_t current_process_stack = USER_STACK_TOP;

void* elf_load_file(void* file) {
  log(LOG_SERIAL, false, "loading elf module\n");
  Elf32_Ehdr* header = (Elf32_Ehdr*)file;
  log(LOG_SERIAL, false, "Type %s%s%s\n",
      header->e_ident[4] == 1 ? "32bit " : "64bit ",
      header->e_ident[5] == 1 ? "Little Endian " : "Big endian ",
      header->e_ident[6] == 1 ? "True ELF " : "buggy ELF ");
  if (!elf_check_supported(header)) {
    log(LOG_SERIAL, false, "ELF File cannot be loaded.\n");
    return NULL;
  }
  switch (header->e_type) {
  case ET_EXEC:
    log(LOG_SERIAL, false, "executable elf found\n");
    // TODO : Implement
    return NULL;
  case ET_REL:
    log(LOG_SERIAL, false, "relocatable elf found\n");
    return elf_load_rel(header);
  }
  return NULL;
}

#define PROC_STACK_BOTTOM 0x20000000

int exec_elf(char* path, char** argv) {
  uint32_t seg_begin, seg_end;
  uint32_t argc;
  fs_node_t* file = vfs_open(path, "r");
  if (!file) {
    return -1;
  }
  log(LOG_SERIAL, false, "file found\n");
  void* stack = kmalloc(PROC_STACK_BOTTOM);
  uint32_t sp = PROC_STACK_BOTTOM;
  for (argc = 0; argv[argc]; argc++) {
    if (argc >= MAXARGC) {
      return -1;
    }
  }
  int size = get_file_size_fs(file);
  void* file_buffer = kmalloc(size);
  read_fs(file, 0, size, file_buffer);
  Elf32_Ehdr* head = file_buffer;
  Elf32_ProgramHeader* prgm_head = (void*)head + head->e_phoff;
  if (elf_check_file(head) == false) {
    log(LOG_SERIAL, false, "invalid ELf executable\n");
    return -1;
  }
  for (int i = 0; i < head->e_phnum; i++) {
    if (prgm_head->p_type == PT_LOAD) {
      seg_begin = prgm_head->p_vaddr;
      seg_end = seg_begin + prgm_head->p_memsz;
      // should allocate region in current process
    }
  }

  return 0;
}

int elf_exec(char* path, int argc, char** argv, char** env) {
  fs_node_t* file = vfs_open(path, "r");
  if (!file) {
    return -1;
  }
  for (uint32_t x = 0x30000000; x < 0x30000000 + file->length; x += 0x1000) {
    paging_alloc_pages(x, 1);
  }
  Elf32_Ehdr* header = (Elf32_Ehdr*)0x30000000;
  read_fs(file, 0, file->length, (uint8_t*)header);
  current_process_name = kmalloc(strlen(path) + 1);
  memcpy(current_process_name, path, strlen(path) + 1);
  // current_process->name = malloc(strlen(path) + 1);
  // memcpy(current_process->name, path, strlen(path) + 1);
  if (header->e_ident != ELFMAG0 || header->e_ident[1] != ELFMAG1 ||
      header->e_ident[2] != ELFMAG2 || header->e_ident[3] != ELFMAG3) {
    printf("Fatal: Not a valid ELF executable.\n");
    for (uint32_t x = 0x30000000; x < 0x30000000 + file->length; x += 0x1000) {
      paging_unmap_page(x);
    }
    close_fs(file);
    return -1;
  }
  for (uint32_t x = 0; x < (uint32_t)header->e_shentsize * header->e_shnum;
       x += header->e_shentsize) {
    Elf32_Shdr* shdr = (Elf32_Shdr*)((uint32_t)header + (header->e_shoff + x));
    if (shdr->sh_addr) {
      // TODO : if is loadable section, set entry of process to shdr->sh_addr
      /*if (shdr->sh_addr < current_process->image.entry){
              current_process->image.entry = shdr->sh_addr;
      }*/
      /*if (shdr->sh_addr + shdr->sh_size - current_process->image.entry >
      current_process->image.size) {
              // We also store the total size of the memory region used by the
      application current_process->image.size = shdr->sh_addr + shdr->sh_size -
      current_process->image.entry;
      }*/
      for (uint32_t i = 0; i < shdr->sh_size + 0x2000; i += 0x1000) {
        paging_alloc_pages(shdr->sh_addr + i, 1);
      }
      if (shdr->sh_type == SHT_NOBITS) {
        /* This is the .bss, zero it */
        memset((void*)(shdr->sh_addr), 0x0, shdr->sh_size);
      } else {
        /* Copy the section into memory */
        memcpy((void*)(shdr->sh_addr),
               (void*)((uint32_t)header + shdr->sh_offset), shdr->sh_size);
      }
    }
  }
  uint32_t entry = (uint32_t)header->e_entry;

  for (uint32_t x = 0x30000000; x < 0x30000000 + file->length; x += 0x1000) {
    paging_unmap_page(x);
  }
  close_fs(file);
  for (uint32_t stack_pointer = USER_STACK_BOTTOM;
       stack_pointer < USER_STACK_TOP; stack_pointer += 0x1000) {
    paging_alloc_pages(stack_pointer, 1);
  }

  /* Collect arguments */
  int envc = 0;
  for (envc = 0; env[envc] != NULL; ++envc)
    ;

  /* Format auxv */
  /*Elf32_auxv auxv[] = {
          {256, 0xDEADBEEF},
          {0, 0}
  };
  int auxvc = 0;
  for (auxvc = 0; auxv[auxvc].id != 0; ++auxvc);*/

  // uintptr_t heap = current_process->image.entry +
  // current_process->image.size; paging_alloc_pages(heap, 1);
  /*char ** argv_ = (char **)heap;
  heap += sizeof(char *) * (argc + 1);
  char ** env_ = (char **)heap;
  heap += sizeof(char *) * (envc + 1);
  void * auxv_ptr = (void *)heap;
  heap += sizeof(Elf32_auxv) * (auxvc);

  for (int i = 0; i < argc; ++i) {
          paging_alloc_pages(heap, 1);
          argv_[i] = (char *)heap;
          memcpy((void *)heap, argv[i], strlen(argv[i]) * sizeof(char) + 1);
          heap += strlen(argv[i]) + 1;
  }
  // Don't forget the NULL at the end of that...
  argv_[argc] = 0;

  for (int i = 0; i < envc; ++i) {
          paging_alloc_pages(heap, 1);
          env_[i] = (char *)heap;
          memcpy((void *)heap, env[i], strlen(env[i]) * sizeof(char) + 1);
          heap += strlen(env[i]) + 1;
  }
  env_[envc] = 0;*/

  // memcpy(auxv_ptr, auxv, sizeof(Elf32_auxv) * (auxvc));

  // current_process->image.heap        = heap; // heap end
  // current_process->image.heap_actual = heap + (0x1000 - heap % 0x1000);
  // current_process->image.user_stack  = USER_STACK_TOP;
  /*while (current_process->fds->length < 3) {
          process_append_fd((process_t *)current_process, NULL);
  }*/

  // current_process->image.start = entry;

  // enter_user_jmp(entry, argc, argv_, USER_STACK_TOP);

  /* We should never reach this code */
  return -1;
}
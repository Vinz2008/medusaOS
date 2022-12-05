#include <kernel/elf.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <kernel/kmalloc.h>

static inline Elf32_SectionHeader *elf_sheader(Elf32_EHeader* header) {
	return (Elf32_SectionHeader*)((int)header + header->e_shoff);
}

static inline Elf32_SectionHeader* elf_section(Elf32_EHeader* header, int idx) {
	return &elf_sheader(header)[idx];
}

static inline char *elf_str_table(Elf32_EHeader* header) {
	if(header->e_shstrndx == SHN_UNDEF) return NULL;
	return (char *)header + elf_section(header, header->e_shstrndx)->sh_offset;
}

static inline char *elf_lookup_string(Elf32_EHeader* header, int offset) {
	char *strtab = elf_str_table(header);
	if(strtab == NULL) return NULL;
	return strtab + offset;
}

bool elf_check_file(Elf32_EHeader* header){
    if (!header) return false;
    if (header->e_ident[EI_MAG0] != ELFMAG0){
        log(LOG_SERIAL, false, "ERROR : ELF Header EI_MAG0 incorrect.\n");
        return false;
    }
    if (header->e_ident[EI_MAG1] != ELFMAG1){
        log(LOG_SERIAL, false, "ERROR : ELF Header EI_MAG1 incorrect.\n");
        return false;
    }
    if (header->e_ident[EI_MAG2] != ELFMAG2){
        log(LOG_SERIAL, false, "ERROR : ELF Header EI_MAG2 incorrect.\n");
        return false;
    }
    if (header->e_ident[EI_MAG3] != ELFMAG3){
        log(LOG_SERIAL, false, "ERROR : ELF Header EI_MAG3 incorrect.\n");
        return false;
    }
    return true;
}

bool elf_check_supported(Elf32_EHeader* header) {
	if(!elf_check_file(header)) {
		log(LOG_SERIAL, false, "ERROR : Invalid ELF File.\n");
		return false;
	}
	if(header->e_ident[EI_CLASS] != ELFCLASS32) {
		log(LOG_SERIAL, false, "ERROR : Unsupported ELF File Class.\n");
		return false;
	}
	if(header->e_ident[EI_DATA] != ELFDATA2LSB) {
		log(LOG_SERIAL, false, "ERROR : Unsupported ELF File byte order.\n");
		return false;
	}
	if(header->e_machine != EM_386) {
		log(LOG_SERIAL, false, "ERROR : Unsupported ELF File target.\n");
		return false;
	}
	if(header->e_ident[EI_VERSION] != EV_CURRENT) {
		log(LOG_SERIAL, false, "ERROR : Unsupported ELF File version.\n");
		return false;
	}
	if(header->e_type != ET_REL && header->e_type != ET_EXEC) {
		log(LOG_SERIAL, false, "ERROR : Unsupported ELF File type.\n");
		return false;
	}
	return true;
}

void* elf_lookup_symbol(const char* name){
    return NULL; // minimal implementation
}


static int elf_get_symval(Elf32_EHeader* header, int table, uint8_t idx){
    if(table == SHN_UNDEF || idx == SHN_UNDEF) return 0;
    Elf32_SectionHeader* symtab = elf_section(header, table);
    uint32_t symtab_entries = symtab->sh_size / symtab->sh_entsize;
    if(idx >= symtab_entries) {
		log(LOG_SERIAL, false, "Symbol Index out of Range (%d:%u).\n", table, idx);
		return ELF_RELOC_ERR;
	}
	int symaddr = (int)header + symtab->sh_offset;
	Elf32_Sym *symbol = &((Elf32_Sym *)symaddr)[idx];
    if(symbol->st_shndx == SHN_UNDEF) {
		// External symbol, lookup value
		Elf32_SectionHeader* strtab = elf_section(header, symtab->sh_link);
		const char *name = (const char *)header + strtab->sh_offset + symbol->st_name;

		extern void *elf_lookup_symbol(const char *name);
		void *target = elf_lookup_symbol(name);

		if(target == NULL) {
			// Extern symbol not found
			if(ELF32_ST_BIND(symbol->st_info) & STB_WEAK) {
				// Weak symbol initialized as 0
				return 0;
			} else {
				log(LOG_SERIAL, false, "Undefined External Symbol : %s.\n", name);
				return ELF_RELOC_ERR;
			}
		} else {
			return (int)target;
		}

	} else if(symbol->st_shndx == SHN_ABS) {
		// Absolute symbol
		return symbol->st_value;
	} else {
		// Internally defined symbol
		Elf32_SectionHeader* target = elf_section(header, symbol->st_shndx);
		return (int)header + symbol->st_value + target->sh_offset;
	}
}

static int elf_load_stage1(Elf32_EHeader* header) {
	Elf32_SectionHeader* shdr = elf_sheader(header);
	unsigned int i;
	// Iterate over section headers
	for(i = 0; i < header->e_shnum; i++) {
		Elf32_SectionHeader* section = &shdr[i];
		// If the section isn't present in the file
		if(section->sh_type == SHT_NOBITS) {
			// Skip if it the section is empty
			if(!section->sh_size) continue;
			// If the section should appear in memory
			if(section->sh_flags & SHF_ALLOC) {
				// Allocate and zero some memory
				void *mem = kmalloc(section->sh_size);
				memset(mem, 0, section->sh_size);
				// Assign the memory offset to the section offset
				section->sh_offset = (int)mem - (int)header;
				log(LOG_SERIAL, false, "Allocated memory for a section (%ld).\n", section->sh_size);
			}
		}
	}
	return 0;
}

static int elf_load_stage2(Elf32_EHeader* header) {
	Elf32_SectionHeader* shdr = elf_sheader(header);

	unsigned int i, idx;
	// Iterate over section headers
	for(i = 0; i < header->e_shnum; i++) {
		Elf32_SectionHeader *section = &shdr[i];
		// If this is a relocation section
		if(section->sh_type == SHT_REL) {
			// Process each entry in the table
			for(idx = 0; idx < section->sh_size / section->sh_entsize; idx++) {
				Elf32_Rel *reltab = &((Elf32_Rel *)((int)header + section->sh_offset))[idx];
				int result = elf_do_reloc(header, reltab, section);
				// On error, display a message and return
				if(result == ELF_RELOC_ERR) {
					log(LOG_SERIAL, false, "Failed to relocate symbol.\n");
					return ELF_RELOC_ERR;
				}
			}
		}
	}
	return 0;
}

static inline void *elf_load_rel(Elf32_EHeader* header) {
	int result;
	result = elf_load_stage1(header);
	if(result == ELF_RELOC_ERR) {
		log(LOG_SERIAL, false, "Unable to load ELF file.\n");
		return NULL;
	}
	result = elf_load_stage2(header);
	if(result == ELF_RELOC_ERR) {
		log(LOG_SERIAL, false, "Unable to load ELF file.\n");
		return NULL;
	}
	// TODO : Parse the program header (if present)
	return (void *)header->e_entry;
}


int elf_do_reloc(Elf32_EHeader* header, Elf32_Rel *rel, Elf32_SectionHeader* reltab) {
	Elf32_SectionHeader* target = elf_section(header, reltab->sh_info);

	int addr = (int)header + target->sh_offset;
	int *ref = (int *)(addr + rel->r_offset);
    	// Symbol value
	int symval = 0;
	if(ELF32_R_SYM(rel->r_info) != SHN_UNDEF) {
		symval = elf_get_symval(header, reltab->sh_link, ELF32_R_SYM(rel->r_info));
		if(symval == ELF_RELOC_ERR) return ELF_RELOC_ERR;
	}
    	// Relocate based on type
	switch(ELF32_R_TYPE(rel->r_info)) {
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
			log(LOG_SERIAL, false, "Unsupported Relocation Type (%d).\n", ELF32_R_TYPE(rel->r_info));
			return ELF_RELOC_ERR;
	}
	return symval;
}

void *elf_load_file(void *file) {
    log(LOG_SERIAL, false, "loading elf module\n");
	Elf32_EHeader *header = (Elf32_EHeader *)file;
	if(!elf_check_supported(header)) {
		log(LOG_SERIAL, false, "ELF File cannot be loaded.\n");
		return NULL;
	}
	switch(header->e_type) {
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
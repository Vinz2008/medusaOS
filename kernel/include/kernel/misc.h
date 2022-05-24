struct _pt_regs {
	/** Stack Segment */
	uint16_t ss;
	/** Data segment */
	uint16_t gs;
	/** Data segment */
	uint16_t fs;
	/** Data segment */
	uint16_t es;
	/** Data segment */
	uint16_t ds;
	/** General-Purpose register : stack pointer */
	uint32_t esp;
	/** General-Purpose register : destination pointer */
	uint32_t edi;
	/** General-Purpose register : source pointer */
	uint32_t esi;
	/** General-Purpose register : stack base pointer */
	uint32_t ebp;
	/** General-Purpose register */
	uint32_t ebx;
	/** General-Purpose register */
	uint32_t edx;
	/** General-Purpose register */
	uint32_t ecx;
	/** General-Purpose register */
	uint32_t eax;
	/** Instruction pointer */
	uint32_t eip;
	/** Code segment register */
	uint16_t cs;
	uint32_t eflags;
} __attribute__((packed));

typedef struct _pt_regs pt_regs;
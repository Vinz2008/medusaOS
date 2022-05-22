typedef struct {
	uint16_t    isr_low;
	uint16_t    kernel_cs;
	uint8_t     reserved;
	uint8_t     attributes;
	uint16_t    isr_high;
} __attribute__((packed)) idt_entry_t;

/*__attribute__((aligned(0x10))) 
static idt_entry_t idt[256];*/

typedef struct {
	uint16_t	limit;
	uint32_t	base;
} __attribute__((packed)) idtr_t;


//static idtr_t idtr;

typedef struct{
   uint16_t offset_1;        
   uint16_t selector;
   uint8_t  zero;           
   uint8_t  type_attributes;
   uint16_t offset_2;        
} idt_desc_t;


    struct cpu_state {
        unsigned int eax;
        unsigned int ebx;
        unsigned int ecx;
        unsigned int edx;
        unsigned int ebp;
        unsigned int esp;
    } __attribute__((packed));

    struct stack_state {
        unsigned int error_code;
        unsigned int eip;
        unsigned int cs;
        unsigned int eflags;
    } __attribute__((packed));

    void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags);

    void idt_init(void);
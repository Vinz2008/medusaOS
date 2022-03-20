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

    void interrupt_handler(struct cpu_state cpu, struct stack_state stack, unsigned int interrupt);
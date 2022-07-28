


void schedule(){
	//asm volatile("add $0xc, %esp");
	asm volatile("push %eax");
	asm volatile("push %ebx");
	asm volatile("push %ecx");
	asm volatile("push %edx");
	asm volatile("push %esi");
	asm volatile("push %edi");
	asm volatile("push %ebp");
	asm volatile("push %ds");
	asm volatile("push %es");
	asm volatile("push %fs");
	asm volatile("push %gs");

	//asm volatile("mov %%esp, %%eax":"=a"(c->esp));
    asm volatile("mov %esp, %eax");
	//c = c->next; // TODO it should here pass to next process
	//asm volatile("mov %%eax, %%cr3": :"a"(c->cr3));
    asm volatile("mov %eax, %cr3");
	//asm volatile("mov %%eax, %%esp": :"a"(c->esp));
    asm volatile("mov %eax, %esp");



	asm volatile("pop %gs");
	asm volatile("pop %fs");
	asm volatile("pop %es");
	asm volatile("pop %ds");
	asm volatile("pop %ebp");
	asm volatile("pop %edi");
	asm volatile("pop %esi");
	asm volatile("out %%al, %%dx": :"d"(0x20), "a"(0x20)); // send EoI to master PIC
	asm volatile("pop %edx");
	asm volatile("pop %ecx");
	asm volatile("pop %ebx");
	asm volatile("pop %eax");
	asm volatile("iret");
}
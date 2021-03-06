#include "protect.h"
#include "type.h"
#include "global.h"
#include "string.h"
#include "print.h"
#include "irqhandler.h"

void out_byte(uint16_t port, uint8_t value){
    __asm__ __volatile__(
        "out    %%al,   %%dx\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        :
        :"d"(port), "a"(value)
    );
}

uint8_t in_byte(uint16_t port){
    uint8_t data;
    __asm__ __volatile__(
        "xor    %%eax,  %%eax\n\t"
        "in     %%dx,   %%al\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        :"=a"(data)
        :"d"(port)
    );
    return data;
}

void port_read_8(uint8_t port, void* buf, int size){
    __asm__ __volatile__(
        "cld\n\t"
        "rep\n\t"
        "insb"
        :
        :"d"(port), "D"(buf), "c"(size)
    );
}

void port_write_8(uint8_t port, void* buf, int size){
    __asm__ __volatile__(
        "cld\n\t"
        "rep\n\t"
        "outsb"
        :
        :"d"(port), "D"(buf), "c"(size)
    );
}

void port_read_16(uint16_t port, void* buf, int size){
    __asm__ __volatile__(
        "shrl   $1,     %%ecx\n\t"
        "cld\n\t"
        "rep\n\t"
        "insw"
        :
        :"d"(port), "D"(buf), "c"(size)
    );
}

void port_write_16(uint16_t port, void* buf, int size){
    __asm__ __volatile__(
        "shrl   $1,     %%ecx\n\t"
        "cld\n\t"
        "rep\n\t"
        "outsw"
        :
        :"d"(port), "S"(buf), "c"(size)
    );
}

void* seg2phyaddr(uint16_t selector){
    Descriptor* descriptor = &gdt[selector >> 3];
    return (void*)((descriptor -> base_high << 24) 
        | (descriptor -> base_mid << 16) 
        | (descriptor ->base_low));
}

void* vir2phyaddr(void* selector_base, void* offset){
    return (void*)((uint32_t)selector_base + (uint32_t)offset);
}

uint32_t get_desc_base(Descriptor* descriptor){
    uint32_t base = 
        (((descriptor->base_high) << 24) & 0xFF000000) |
        (((descriptor->base_mid) << 16) & 0x00FF0000) |
        (descriptor->base_low & 0x0000FFFF);
    return base;
}

void init_descriptor(Descriptor* descriptor, uint32_t base, uint32_t limit, uint16_t attribute){
    descriptor -> limit_low = limit & 0x0FFFF;
    descriptor -> base_low = base & 0x0FFFF;
    descriptor -> base_mid = (base >> 16) & 0x0FF;
    descriptor -> base_high = (base >> 24) & 0x0FF;
    descriptor -> attr1 = attribute & 0xFF;
    descriptor -> limit_high_attr2 = ((limit >> 16) & 0x0F) | (attribute >> 8) & 0xF0;
}

void init_idt_descriptor(unsigned char _vector, 
                        uint8_t type, 
                        interrupt_handler handler, 
                        unsigned char privilege){
    Gate* gate_ptr = &idt[_vector];
    uint32_t base = (uint32_t)handler;
    gate_ptr -> offset_low = base & 0xFFFF;
    gate_ptr -> selector = SELECTOR_MEMC_0;
    gate_ptr -> dcount = 0;
    gate_ptr -> attr = type | (privilege << 5);
    gate_ptr -> offset_high = (base >> 16) & 0xFFFF;
}

void dummy_irq(int irq){
    printString("irq: ", -1);
    printInt32(irq);
    printString("\n", -1);
}

void set_irq_handler(int vec_no, irq_handler handler){
    irq_table[vec_no] = handler;
}

void init_8259A(){
    out_byte(INT_M_CTL, 0x11);                  //icw1 => master8259A
    out_byte(INT_S_CTL, 0x11);                  //icw1 => slave8259A
    out_byte(INT_M_CTLMASK, INT_VECTOR_IRQ0);   //icw2 => master8259A, set its interrupt vector = 0x20
    out_byte(INT_S_CTLMASK, INT_VECTOR_IRQ8);   //icw2 => slave8259A, set its interrupt vector = 0x28
    out_byte(INT_M_CTLMASK, 0x4);               //icw3 => master8259A
    out_byte(INT_S_CTLMASK, 0x2);               //icw3 => slave8259A
    out_byte(INT_M_CTLMASK, 0x1);               //icw4 => master8259A
    out_byte(INT_S_CTLMASK, 0x1);               //icw4 => slave8259A

    out_byte(INT_M_CTLMASK, 0xF8);              //ocw1 => master8259A  
    out_byte(INT_S_CTLMASK, 0xBF);              //ocw1 => slave8259A

    //init irq
    for(int i = 0; i < IRQ_NUMBER; i++){//dummy irq
        irq_table[i] = dummy_irq;
    }
}

void init_interrupt(){
    init_8259A();
    
    //exception
    init_idt_descriptor(INT_VECTOR_DIVIDE, DA_386IGate, divide_error, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_DEBUG, DA_386IGate, single_step_exception, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_NMI, DA_386IGate, nmi, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_BREAKPOINT, DA_386IGate, breakpoint_exception, PRIVILEGE_USER);
    init_idt_descriptor(INT_VECTOR_OVERFLOW, DA_386IGate, overflow, PRIVILEGE_USER);
    init_idt_descriptor(INT_VECTOR_BOUNDS, DA_386IGate, bounds_check, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_INVAL_OP, DA_386IGate, inval_opcode, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_COPROC_NOT, DA_386IGate, copr_not_available, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_DOUBLE_FAULT, DA_386IGate, double_fault, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_COPROC_SEG, DA_386IGate, copr_seg_overrun, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_INVAL_TSS, DA_386IGate, inval_tss, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_SEG_NOT, DA_386IGate, segment_not_present, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_STACK_FAULT, DA_386IGate, stack_exception, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_PROTECTION, DA_386IGate, general_protection, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_PAGE_FAULT, DA_386IGate, page_fault, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_COPROC_ERR, DA_386IGate, copr_error, PRIVILEGE_KERNEL);
    
    //hardware interrupt
    init_idt_descriptor(INT_VECTOR_IRQ0 + 0, DA_386IGate, hwint00, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_IRQ0 + 1, DA_386IGate, hwint01, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_IRQ0 + 2, DA_386IGate, hwint02, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_IRQ0 + 3, DA_386IGate, hwint03, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_IRQ0 + 4, DA_386IGate, hwint04, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_IRQ0 + 5, DA_386IGate, hwint05, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_IRQ0 + 6, DA_386IGate, hwint06, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_IRQ0 + 7, DA_386IGate, hwint07, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_IRQ8 + 0, DA_386IGate, hwint08, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_IRQ8 + 1, DA_386IGate, hwint09, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_IRQ8 + 2, DA_386IGate, hwint10, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_IRQ8 + 3, DA_386IGate, hwint11, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_IRQ8 + 4, DA_386IGate, hwint12, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_IRQ8 + 5, DA_386IGate, hwint13, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_IRQ8 + 6, DA_386IGate, hwint14, PRIVILEGE_KERNEL);
    init_idt_descriptor(INT_VECTOR_IRQ8 + 7, DA_386IGate, hwint15, PRIVILEGE_KERNEL);

    //system call
    init_idt_descriptor(INT_VECTOR_SYSTEM_CALL, DA_386IGate, system_call, PRIVILEGE_USER);
}

void init_tss_descriptor(){
    memset(&tss, 0, sizeof(TSS));
    tss.ss0 = SELECTOR_MEMD_0;
    tss.ss1 = SELECTOR_MEMD_1;
    tss.ss2 = SELECTOR_MEMD_2;
    tss.esp0 = 0xFFF;
    init_descriptor(
        &gdt[INDEX_TSS], 
        (uint32_t)vir2phyaddr(seg2phyaddr(SELECTOR_MEMD_0), &tss), 
        sizeof(tss) - 1, 
        DA_386TSS
    );
    tss.iobase = sizeof(tss);
}

void init_ldt(){
    for(uint32_t i = 0; i < MAX_PROCESS_NUM; i++){
        pcb_table[i].ldt_selector = SELECTOR_LDT_FIRST + (i << 3);
        init_descriptor(
            &gdt[INDEX_LDT_FIRST + i], 
            (uint32_t)(&(pcb_table[i].ldts)), 
            LDT_SIZE * sizeof(Descriptor) - 1, 
            DA_LDT
        );
    }
}

void exception_handler(int vec_no, int err_code, int eip, int cs, int eflags){
	char err_description[][64] = {	
        "#DE Divide Error",
		"#DB RESERVED",
		"—  NMI Interrupt",
		"#BP Breakpoint",
		"#OF Overflow",
		"#BR BOUND Range Exceeded",
		"#UD Invalid Opcode (Undefined Opcode)",
		"#NM Device Not Available (No Math Coprocessor)",
		"#DF Double Fault",
		"    Coprocessor Segment Overrun (reserved)",
		"#TS Invalid TSS",
		"#NP Segment Not Present",
		"#SS Stack-Segment Fault",
		"#GP General Protection",
		"#PF Page Fault",
		"—  (Intel reserved. Do not use.)",
		"#MF x87 FPU Floating-Point Error (Math Fault)",
		"#AC Alignment Check",
		"#MC Machine Check",
		"#XF SIMD Floating-Point Exception"
	};
    upRollScreen();
    printString(err_description[vec_no], -1);
    upRollScreen();
    printString("vec: ", -1);
    printInt32(vec_no);
    upRollScreen();
    printString("err code: ", -1);
    printInt32(err_code);
    upRollScreen();
    printString("cs: ", -1);
    printInt32(cs);
    upRollScreen();
    printString("eip: ", -1);
    printInt32(eip);
    upRollScreen();
    printString("eflags: ", -1);
    printInt32(eflags);
    upRollScreen();

}

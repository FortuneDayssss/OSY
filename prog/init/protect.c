#include "protect.h"
#include "type.h"
#include "global.h"
#include "string.h"

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

void* seg2phyaddr(uint16_t selector){
    Descriptor* descriptor = &gdt[selector >> 3];
    return (void*)((descriptor -> base_high << 24) 
        | (descriptor -> base_mid << 16) 
        | (descriptor ->base_low));
}

void* vir2phyaddr(void* selector_base, void* offset){
    return (void*)((uint32_t)selector_base + (uint32_t)offset);
}

void init_descriptor(Descriptor* descriptor, uint32_t base, uint32_t limit, uint16_t attribute){
    descriptor -> limit_low = limit & 0x0FFFF;
    descriptor -> base_low = base & 0x0FFFF;
    descriptor -> base_mid = (base >> 16) & 0x0FF;
    descriptor -> base_high = (base >> 24) & 0x0FF;
    descriptor -> attr1 = attribute & 0xFF;
    descriptor -> limit_high_attr2 = ((limit >> 16) & 0x0F) | (attribute >> 8) & 0xF0;
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

    out_byte(INT_M_CTLMASK, 0xFD);              //ocw1 => master8259A **open keybord interrupt(irq1)    
    out_byte(INT_S_CTLMASK, 0xFF);              //ocw1 => slave8259A

    //todo: init irq
}

void init_interrupt(){
    init_8259A();

}

void init_tss(){
    memset(&tss, 0, sizeof(TSS));
    tss.ss0 = SELECTOR_MEMD;
    init_descriptor(
        &gdt[INDEX_TSS], 
        (uint32_t)vir2phyaddr(seg2phyaddr(SELECTOR_MEMD), &tss), 
        sizeof(tss) - 1, 
        DA_386TSS
    );
    tss.iobase = sizeof(tss);
}
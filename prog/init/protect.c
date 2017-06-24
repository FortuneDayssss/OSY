#include "protect.h"
#include "type.h"

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
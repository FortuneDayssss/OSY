#ifndef _GLOBAL_H__
#define _GLOBAL_H__
#include "type.h"
#include "protect.h"
#include "process.h"

//GDT
extern uint8_t      gdt_ptr[6];//limit(2Byte)  Base(4Byte)
extern Descriptor   gdt[GDT_SIZE];

//IDT
extern uint8_t      idt_ptr[6];//limit(2Byte)  Base(4Byte)
extern Gate         idt[IDT_SIZE];

//TSS
extern TSS          tss;

//process
extern PCB          pcb_table[MAX_PROCESS_NUM];
extern PCB*         current_process;

//screen
extern int dispPos;

//irq
extern irq_handler  irq_table[IRQ_NUMBER];

#endif
#include "global.h"
#include "protect.h"
#include "type.h"

//GDT
uint8_t     gdt_ptr[6];//limit(2Byte)  Base(4Byte)
Descriptor  gdt[GDT_SIZE];

//IDT
uint8_t     idt_ptr[6];//limit(2Byte)  Base(4Byte)
Gate        idt[IDT_SIZE];

//TSS
TSS         tss;

//process
PCB         pcb_table[MAX_PROCESS_NUM];
PCB*        current_process;

//screen
int dispPos = 0;

//irq
irq_handler irq_table[IRQ_NUMBER];
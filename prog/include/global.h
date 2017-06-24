#ifndef _GLOBAL_H__
#define _GLOBAL_H__
#include "type.h"
#include "const.h"
#include "protect.h"

//GDT
extern uint8_t     gdt_ptr[6];//limit(2Byte)  Base(4Byte)
extern Descriptor  gdt[GDT_SIZE];

//IDT
extern uint8_t     idt_ptr[6];//limit(2Byte)  Base(4Byte)
extern Gate        idt[IDT_SIZE];

//TSS
extern TSS         tss;

//process
//TODO

//screen
extern int dispPos;

#endif
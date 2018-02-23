#ifndef _GLOBAL_H__
#define _GLOBAL_H__
#include "type.h"
#include "protect.h"
#include "process.h"
#include "keyboard.h"
#include "tty.h"

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
extern PCB*         next_process;

//screen
extern int          dispPos;

//irq
extern irq_handler  irq_table[IRQ_NUMBER];

//system ticks
extern uint64_t     sys_ticks;

//tty
extern TTY          tty_table[NR_TTYS];
extern TTY*         current_tty;

//device - driver map
//same as index of dd_map (in global.c)
#define NO_DEV      0
#define DEV_FLOPY   1
#define DEV_CDROM   2
#define DEV_HD      3
#define DEV_TTY     4
#define DEV_SCSI    5
extern uint32_t dd_map[];

#endif
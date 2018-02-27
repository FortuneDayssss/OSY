#ifndef _GLOBAL_H__
#define _GLOBAL_H__
#include "type.h"
#include "protect.h"
#include "process.h"
#include "keyboard.h"
#include "tty.h"
#include "fs.h"

#define min(a,b) ((a) < (b) ? (a) : (b))

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

// screen
extern int          dispPos;

// irq
extern irq_handler  irq_table[IRQ_NUMBER];

// system ticks
extern uint64_t     sys_ticks;

// tty
extern TTY          tty_table[NR_TTYS];
extern TTY*         current_tty;

// fs
extern File_Descriptor      fd_table[NR_FILE_DESCRIPTOR];
extern INode                inode_table[NR_INODE];
extern Super_Block          super_block_table[NR_SUPER_BLOCK];
extern INode*               root_inode;

// device - driver map
// same as index of dd_map (in global.c)
#define NO_DEV      0
#define DEV_FLOPY   1
#define DEV_CDROM   2
#define DEV_HD      3
#define DEV_TTY     4
#define DEV_SCSI    5

#define MAKE_DEV(a,b)  ((a << 8) | b)
#define GET_DEV_MAJOR(a)    ((a >> 8) & 0xFF)
#define GET_DEV_MINOR(a)    (a & 0xFF)

#define INODE_INVALID   0
#define INODE_ROOT      1

extern uint32_t dd_map[];

#endif
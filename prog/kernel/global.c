#include "global.h"
#include "protect.h"
#include "process.h"
#include "type.h"
#include "tty.h"
#include "mm.h"

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
PCB*        next_process;

//screen
int dispPos = 0;

//irq
irq_handler irq_table[IRQ_NUMBER];

//system ticks
uint64_t    sys_ticks = 0;

//tty
TTY tty_table[NR_TTYS];
TTY* current_tty;

// fs
File_Descriptor     fd_table[NR_FILE_DESCRIPTOR];
INode               inode_table[NR_INODE];
Super_Block         super_block_table[NR_SUPER_BLOCK];
INode*              root_inode;

// mm
Page_Table* kernel_page_table = (Page_Table*)(0x00200000);


//device - driver map
uint32_t dd_map[] = {
    PID_INVALID,    //0: unused
    PID_INVALID,    //1: reserved for floppy driver
    PID_INVALID,    //2: reserved for cdrom driver
    PID_HD,         //3: hard disk driver
    PID_TTY,        //4: tty
    PID_INVALID     //5: reserved for scsi disk driver
};
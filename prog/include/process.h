#ifndef _PROCESS_H__
#define _PROCESS_H__

#define MAX_PROCESS_NUM     10
#define STACK_SIZE          4*1024

#define PROCESS_RUNNING         0   //running
#define PROCESS_INTERRUPTED     1   //interruptable sleep
#define PROCESS_UNINTERRUPTABLE 2   //uninterruptable sleep
#define PROCESS_STOPPED         3   //stopped
#define PROCESS_DEAD            4   //zombie
#define PROCESS_EMPTY           5   //(for test) avaliable pcb
#define PROCESS_READY           6   //for test


typedef struct{
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t kernel_esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t return_address;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
}Register_Frame;

typedef struct{
    Register_Frame  registers;
    uint16_t        ldt_selector;
    Descriptor      ldt[LDT_SIZE];
    uint8_t         stack0[STACK_SIZE];
    uint8_t         stack1[STACK_SIZE];
    uint8_t         stack2[STACK_SIZE];
    uint8_t         stack3[STACK_SIZE];


    uint32_t        pid;
    char            name[32];
    uint32_t        state;

    uint32_t        tick;
}PCB;

#endif
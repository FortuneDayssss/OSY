#ifndef _PROCESS_H__
#define _PROCESS_H__
#include "type.h"
#include "protect.h"

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
    uint8_t         stack0[STACK_SIZE];
    uint8_t         stack1[STACK_SIZE];
    uint8_t         stack2[STACK_SIZE];
    uint8_t         stack3[STACK_SIZE];
    
    uint32_t        esp;

    uint32_t        pid;
    char            name[32];
    uint32_t        state;
    uint32_t        tick;
}PCB;

void schedule();                    //process.c
void switch_to_next_process();      //kernel.asm

#endif
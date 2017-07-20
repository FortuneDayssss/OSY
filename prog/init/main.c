#include "protect.h"
#include "global.h"
#include "process.h"
#include "string.h"
#include "print.h"
#include "type.h"

void sleep(int time){
    for(int i = 0; i < time; i++)
        for(int i = 0; i < 10000; i++)
            ;
}

void schedule_output_test(){
    upRollScreen();
    printString("schedule!\n", -1);
}

void test(){
    upRollScreen();
    printString("test!\n", -1);
}

void p1test(){
    while(1){
        printString("p1---   ", -1);
        sleep(10);
        // sleep(100);
    }
}

void p2test(){
    while(1){
        //printString("p2---   ", -1);
        __asm__(
            "int    $0x80\n\t"
        );
        sleep(10);
        // sleep(100);
    }
}

int init_process(PCB* pcb, void (*startAddr)){
    uint32_t* stackPointer = (uint32_t*)(&pcb->stack0[STACK_SIZE - 4]);//stack0 top
    *stackPointer = SELECTOR_MEMD_3;
    stackPointer--;
    *stackPointer = (uint32_t)(&pcb->stack3[STACK_SIZE - 4]);//stack3 top
    stackPointer--;
    *stackPointer = 0x1202;//IF = 1, IOPL = 1
    stackPointer--;
    *stackPointer = SELECTOR_MEMC_3;
    stackPointer--;
    *stackPointer = (uint32_t)startAddr;
    stackPointer--;
    *stackPointer = 0;//eax
    stackPointer--;
    *stackPointer = 0;//ecx
    stackPointer--;
    *stackPointer = 0;//edx
    stackPointer--;
    *stackPointer = 0;//ebx
    stackPointer--;
    *stackPointer = (uint32_t)(&pcb->stack3[STACK_SIZE - 4]);//esp
    stackPointer--;
    *stackPointer = 0;//ebp
    stackPointer--;
    *stackPointer = 0;//esi
    stackPointer--;
    *stackPointer = 0;//edi
    pcb->esp = (uint32_t)stackPointer;
    pcb->state = PROCESS_READY;
    pcb->tick = 20;
}

int init_dummy_process(PCB* pcb){
    pcb->state = PROCESS_EMPTY;
}

int main(){
    printString("main\n", -1);
    for(int i = 0; i < MAX_PROCESS_NUM; i++){
        init_dummy_process(&pcb_table[i]);
    }
    init_process(&pcb_table[0], p1test);
    init_process(&pcb_table[1], p2test);
    pcb_table[0].state = PROCESS_RUNNING;

    current_process = pcb_table;

    int a = (int)p1test;
    printInt32(a);
    upRollScreen();

    change_to_user_mode();

    while(1){}
}
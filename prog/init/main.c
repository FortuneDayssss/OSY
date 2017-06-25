#include "protect.h"
#include "global.h"
#include "process.h"
#include "string.h"
#include "print.h"
#include "type.h"

void change_to_user_mode();//test, todo

void test(){
    printString("test\n", -1);
}

void p1test(){
    while(1){
        printString("p1---\n", -1);
    }
}

void p2test(){
    while(1){
        printString("p2---", -1);
    }
}

int main(){
    printString("main\n", -1);

    //init pcb
    for(int i = 0; i < MAX_PROCESS_NUM; i++){
        PCB* p = &(pcb_table[i]);
        
        //ldt
        p->ldt_selector = SELECTOR_LDT_FIRST + 8 * i;
        memcpy(&(p->ldt[0]), &(gdt[SELECTOR_MEMC >> 3]), sizeof(Descriptor));
        memcpy(&(p->ldt[1]), &(gdt[SELECTOR_MEMD >> 3]), sizeof(Descriptor));
        p->ldt[0].attr1 = DA_C | PRIVILEGE_USER << 5;
        p->ldt[1].attr1 = DA_DRW | PRIVILEGE_USER << 5;
        
        //register
        p->registers.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | SA_RPL3;
        p->registers.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | SA_RPL3;
        p->registers.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | SA_RPL3;
        p->registers.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | SA_RPL3;
        p->registers.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | SA_RPL3;
        p->registers.gs = (SELECTOR_VIDEO & SA_RPL_MASK) | SA_RPL3;
        p->registers.esp = (uint32_t)(&(p->stack3[STACK_SIZE - 4]));
        p->registers.eip = 0;
        p->registers.eflags = 0x1202;//IF = 1, IOPL = 1
        
        //process state
        p->pid = i;
        //p->name;
        p->state = PROCESS_EMPTY;
        p->tick = 0;
    }

    //process for test
    pcb_table[0].state = PROCESS_READY;
    pcb_table[0].registers.eip = (uint32_t)(p1test);
    pcb_table[1].state = PROCESS_READY;
    pcb_table[1].registers.eip = (uint32_t)(p1test);

    current_process = pcb_table;

    int a = (int)p1test;
    printInt32(a);
    upRollScreen();

    change_to_user_mode();

    while(1){}
}
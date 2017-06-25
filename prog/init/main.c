#include "protect.h"
#include "global.h"
#include "process.h"
#include "string.h"
#include "print.h"
#include "type.h"

int main(){
    printString("main\n", -1);

    for(int i = 0; i < MAX_PROCESS_NUM; i++){
        PCB* p = &(pcb_table[i]);
        
        //ldt
        p->ldt_selector = SELECTOR_LDT_FIRST + sizeof(Descriptor) * i;
        memcpy(&p->ldt[0], &gdt[SELECTOR_MEMC >> 3], sizeof(Descriptor));
        memcpy(&p->ldt[1], &gdt[SELECTOR_MEMD >> 3], sizeof(Descriptor));
        p->ldt[0].attr1 = DA_C | PRIVILEGE_USER << 5;
        p->ldt[1].attr1 = DA_C | PRIVILEGE_USER << 5;
        
        //register
        p->registers.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | SA_RPL3;
        p->registers.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | SA_RPL3;
        p->registers.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | SA_RPL3;
        p->registers.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | SA_RPL3;
        p->registers.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | SA_RPL3;
        p->registers.gs = (SELECTOR_VIDEO & SA_RPL_MASK) | SA_RPL3;
        p->registers.esp = (uint32_t)(&(p->stack3[STACK_SIZE - 4]));
        p->registers.eip = 0;//todo
        
        //tss
        memset(&(p->tss), 0, sizeof(TSS));
        p->tss.ss0 = SELECTOR_MEMD;
        p->tss.ss1 = SELECTOR_MEMD;
        p->tss.ss2 = SELECTOR_MEMD;
        p->tss.esp0 = (uint32_t)(&(p->stack0[STACK_SIZE - 4]));
        p->tss.esp1 = (uint32_t)(&(p->stack1[STACK_SIZE - 4]));
        p->tss.esp2 = (uint32_t)(&(p->stack2[STACK_SIZE - 4]));
    }

    while(1){}
}

void test(){
    printString("test\n", -1);
}
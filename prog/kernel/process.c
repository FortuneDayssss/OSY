#include "process.h"
#include "global.h"

void block(int pid){//just can use in kernel privilege
    __asm__("cli\n\t" ::);
    
    pcb_table[pid].state = PROCESS_WAITING;

    __asm__(
        "pushf\n\t"
        "pushl  %%cs\n\t"
        "pushl  $2f\n\t"
        "pusha\n\t"
        "call   %1\n\t"         //schedule();
        "subl   $4, %%esp\n\t"  //switch_to_next_process做了平衡clock_handler的栈平衡, 设置32位空位模拟clock_handler
        "jmp    %0\n\t"         //switch_to_next_process
        "2:\n\t"
        "sti"
        :
        :"m"(switch_to_next_process), "m"(schedule)
    );

}

void schedule(){
    next_process = current_process;
    current_process->tick = 200;
    int next_process_index = current_process - pcb_table;

    for(int i = 0; i < MAX_PROCESS_NUM; i++){
        next_process_index = (next_process_index + 1) % MAX_PROCESS_NUM;
        if(pcb_table[next_process_index].state == PROCESS_READY
            || pcb_table[next_process_index].state == PROCESS_STOPPED){
            next_process = &pcb_table[next_process_index];
            if(current_process->state == PROCESS_RUNNING){
                current_process->state = PROCESS_STOPPED;
            }
            next_process->state = PROCESS_RUNNING;
            break;
        }
    }

    // printString("next process:", -1);
    // printInt32(next_process - pcb_table);
    // upRollScreen();
}

uint32_t create_process(void (*startAddr), uint32_t privilege, uint32_t nr_tty){
// int init_process(PCB* pcb, void (*startAddr), int nr_tty){
    int pid = -1;
    for(int i = 0; i < MAX_PROCESS_NUM; i++){
        if(pcb_table[i].state == PROCESS_EMPTY){
            pid = i;
            break;
        }
    }

    PCB* pcb = pcb_table + pid;
    
    if(pid != -1){
        uint32_t* stackPointer = (uint32_t*)(&pcb->stack0[STACK_SIZE - 4]);//stack0 top
        switch(privilege){
            case  PRIVILEGE_USER:
                *stackPointer = SELECTOR_MEMD_3;
                stackPointer--;
                *stackPointer = (uint32_t)(&pcb->stack3[STACK_SIZE - 4]); //stack3 top
                stackPointer--;
                *stackPointer = 0x1202; //IF = 1, IOPL = 1
                stackPointer--;
                *stackPointer = SELECTOR_MEMC_3;
                stackPointer--;
                *stackPointer = (uint32_t)startAddr;
                stackPointer--;
                *stackPointer = 0; //eax
                stackPointer--;
                *stackPointer = 0; //ecx
                stackPointer--;
                *stackPointer = 0; //edx
                stackPointer--;
                *stackPointer = 0; //ebx
                stackPointer--;
                *stackPointer = (uint32_t)(&pcb->stack3[STACK_SIZE - 4]); //esp
                stackPointer--;
                *stackPointer = 0; //ebp
                stackPointer--;
                *stackPointer = 0; //esi
                stackPointer--;
                *stackPointer = 0; //edi
                pcb->esp = (uint32_t)stackPointer;
                pcb->state = PROCESS_READY;
                pcb->tick = 20;
                pcb->tty = tty_table + nr_tty;
                break;
            case PRIVILEGE_KERNEL:
                *stackPointer = SELECTOR_MEMD_0;
                stackPointer--;
                *stackPointer = (uint32_t)(&pcb->stack0[STACK_SIZE - 4]); //stack3 top
                stackPointer--;
                *stackPointer = 0x1202; //IF = 1, IOPL = 1
                stackPointer--;
                *stackPointer = SELECTOR_MEMC_0;
                stackPointer--;
                *stackPointer = (uint32_t)startAddr;
                stackPointer--;
                *stackPointer = 0; //eax
                stackPointer--;
                *stackPointer = 0; //ecx
                stackPointer--;
                *stackPointer = 0; //edx
                stackPointer--;
                *stackPointer = 0; //ebx
                stackPointer--;
                *stackPointer = (uint32_t)(&pcb->stack0[STACK_SIZE - 4]); //esp
                stackPointer--;
                *stackPointer = 0; //ebp
                stackPointer--;
                *stackPointer = 0; //esi
                stackPointer--;
                *stackPointer = 0; //edi
                pcb->esp = (uint32_t)stackPointer;
                pcb->state = PROCESS_READY;
                pcb->tick = 20;
                pcb->tty = tty_table + nr_tty;
                break;
            default:
                break;
        }
    }

    return pid;
}
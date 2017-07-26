#include "process.h"
#include "global.h"

void schedule(){
    next_process = current_process;
    current_process->tick = 200;
    int next_process_index = 0;
    for(int i = 0; i < MAX_PROCESS_NUM; i++){
        if(current_process == &pcb_table[i]){
            next_process_index = i;
            break;
        }
    }

    for(int i = 0; i < MAX_PROCESS_NUM; i++){
        next_process_index = (next_process_index + 1) % MAX_PROCESS_NUM;
        if(pcb_table[next_process_index].state == PROCESS_READY
            || pcb_table[next_process_index].state == PROCESS_STOPPED){
            next_process = &pcb_table[next_process_index];
            current_process->state = PROCESS_STOPPED;
            next_process->state = PROCESS_RUNNING;
            break;
        }
    }

}
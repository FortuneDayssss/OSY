#include "process.h"
#include "global.h"

void get_next_process(){
    int next_p_index;
    for(int i = 0; i < MAX_PROCESS_NUM; i++){
        if(&pcb_table[i] == current_process){
            next_p_index = i;
        }
    }
    if(next_p_index == 1)
        next_p_index = 0;
    else
        next_p_index = 1;
    next_process = &pcb_table[next_p_index];
}
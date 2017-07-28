#include "global.h"
#include "process.h"


void clock_handler(){
    sys_ticks++;

    current_process->tick--;
    if(current_process->tick > 0)
       return;

    //schedule
	schedule();

    //switch to next process
    __asm__(
        "movl   %%ebp,  %%esp\n\t"
        "popl   %%ebp\n\t"
        "jmp    %0\n\t"
        :
        :"m"(switch_to_next_process)
    );
}
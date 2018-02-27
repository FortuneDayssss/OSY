#include "global.h"
#include "process.h"
#include "protect.h"

void clock_handler();

void init_clock(){
    //init 8253 (frequency ~~= interrupt/10ms)
    out_byte(TIMER_MODE, RATE_GENERATOR);
    out_byte(TIMER0, (uint8_t) (TIMER_FREQ/HZ) );
    out_byte(TIMER0, (uint8_t) ((TIMER_FREQ/HZ) >> 8));

    //init clock irq
    set_irq_handler(CLOCK_IRQ, clock_handler);
}

void clock_handler(){
    sys_ipc_int_send(PID_TTY);

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
#include "protect.h"
#include "global.h"
#include "process.h"
#include "string.h"
#include "print.h"
#include "type.h"
#include "systemcall.h"
#include "tty.h"

void sleep(int time){
    for(int i = 0; i < time; i++)
        for(int i = 0; i < 10000; i++)
            ;
}

void schedule_output_test(){
    upRollScreen();
    printString("schedule!\n", -1);
}

void p1test(){
    while(1){
        ttywrite("EEEEEEJJJJJJEEEEEEJJJJJJ\n", -1);
        sleep(100);
        // sleep(100);
    }
}


void p3test(){
    while(1){
        printString("p3!\n", -1);
        upRollScreen();
        sleep(100);
    }
}

int init_dummy_process(PCB* pcb){
    pcb->state = PROCESS_EMPTY;
}

int main(){
    printString("main\n", -1);
    for(int i = 0; i < MAX_PROCESS_NUM; i++){
        init_dummy_process(&pcb_table[i]);
    }
    int pid = create_process(p1test, PRIVILEGE_USER, 0);
    pcb_table[pid].state = PROCESS_RUNNING;
    create_process(tty_main, PRIVILEGE_KERNEL, 0);
    // create_process(p1test, PRIVILEGE_USER, 0);

    current_process = pcb_table;

    switch_to_user_mode();

    while(1){}
}
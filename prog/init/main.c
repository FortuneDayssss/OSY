#include "protect.h"
#include "global.h"
#include "process.h"
#include "string.h"
#include "print.h"
#include "type.h"
#include "systemcall.h"
#include "tty.h"
#include "ipc.h"
#include "hd.h"

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
    for(int i = 0; i < 5; i++){
        ttywrite("EEEEEEJJJJJJEEEEEEJJJJJJ\n", -1);
    }
    Message msg;
    msg.type = 555;
    msg.msg1.data1 = 10;
    // for(int i = 0; i < 10; i++){
        // sleep(1000);
        ipc_send(PID_HD, &msg);
    // }

    while(1){
        // ttywrite("P1!\n", -1);
        sleep(100);
        sleep(100);
    }
}


void p3test(){
    for(int i = 0; i < 5; i++){
        ttywrite("P3!\n", -1);
        sleep(100);
        sleep(100);
    }
    // pcb_table[1].state = PROCESS_STOPPED;
    Message msg;
    while(1){
        ipc_recv(PID_ANY, &msg);
        ttywrite("P3!!\n", -1);
        sleep(100);
        sleep(100);
    }
}

int init_dummy_process(PCB* pcb){
    pcb->state = PROCESS_EMPTY;
}

extern void hd_read_write(uint32_t drive, uint32_t sector, uint32_t* buf, uint32_t len, uint32_t is_read);

int main(){
    printString("main\n", -1);
    for(int i = 0; i < MAX_PROCESS_NUM; i++){
        init_dummy_process(&pcb_table[i]);
    }
    create_process(tty_main, PRIVILEGE_KERNEL, 0);
    create_process(hd_main, PRIVILEGE_KERNEL, 0);
    create_process(p1test, PRIVILEGE_USER, 0);    
    // create_process(p3test, PRIVILEGE_USER, 0);    
    
    current_process = pcb_table;
    
    switch_to_user_mode();

    while(1){}
}
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
#include "fs.h"
#include "message.h"

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
    // for(int i = 0; i < 3; i++){
    //     ttywrite("p1test process\n", -1);
    // }
    uint32_t buf = 0xBBBBBBBB;
    Message msg;
    msg.type = MSG_HD_READ;
    msg.mdata_hd_read.sector = 0;
    msg.mdata_hd_read.buf_addr = (uint32_t)(&buf);
    msg.mdata_hd_read.len = 4;
    ipc_send(PID_HD, &msg);
    ipc_recv(PID_HD, &msg);
    printString("response:  ", -1);printInt32(msg.mdata_response.status);printString("\n", -1);
    printString("buf data:  ", -1);printInt32(buf);printString("\n", -1);
   

    while(1){
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
    create_process(fs_main, PRIVILEGE_KERNEL, 0);
    // create_process(p1test, PRIVILEGE_USER, 0);
    // create_process(p3test, PRIVILEGE_USER, 0);    
    
    current_process = pcb_table;
    
    switch_to_user_mode();

    while(1){}
}
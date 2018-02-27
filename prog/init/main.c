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
#include "stdio.h"

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

    debug_log("ZZZZZ---");
    test();
    debug_log("AAAAA---");
    ipc_send(PID_HD, &msg);
    debug_log("BBBBB---");
    ipc_recv(PID_HD, &msg);
    printString("response:  ", -1);printInt32(msg.mdata_response.status);printString("\n", -1);
    printString("buf data:  ", -1);printInt32(buf);printString("\n", -1);
   

    while(1){
        sleep(100);
        sleep(100);
    }
}


void p3test(){
    sleep(1000);
    char buf[60] = "hello world tty";
    printString("before open!\n", -1);
    int fd = open("/test_file_2", O_RDWR);
    // int fd = open("/dev_tty0", O_RDWR);
    if(fd != -1){
        printString("open ok!\n", -1);
        printInt32(pcb_table[3].filp_table[fd]->fd_inode->nr_inode);
        int len = read(fd, buf, 5);
        buf[len] = '\0';
        printString("\n", -1);
        debug_log("read from tty finish!");
        printString(buf, -1);
        // buf[len] = '\0';
        // debug_log("read ok!!!");
        // printString("data in /test_file_2: ", -1);printString(buf, -1);printString("\n", -1);
        // close(fd);
    }
    else{
        error_log("open fail");
    }

    while(1){}
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
    create_process(p3test, PRIVILEGE_USER, 0);
    
    current_process = pcb_table;
    
    switch_to_user_mode();

    while(1){}
}
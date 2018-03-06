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
#include "mm.h"

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


void fork_test(){
    int test_data = 1;
    sleep(2000);
    debug_log("aaaaaaaaaaaaa");
    if(!fork()){
        debug_log("child start");
        test_data = 2;
        // Message msg;
        // msg.type = MSG_FS_OPEN;
        // ipc_send(PID_MM, &msg);
        for(int i = 0; i < 10; i++){
            debug_log("child");
            printString("test data: ", -1);printInt32(test_data);upRollScreen();
            sleep(200);
        }
        if(!fork()){
            while(1){
                debug_log("CHILD2222222~~");
                sleep(200);
            }
        }
        else{
            while(1){
                debug_log("CHILD111111~~");
                sleep(200);
            }
        }
        // exit(-10);
    }
    else{
        int exit_status = 0;
        // wait(&exit_status);
        printString("got exit status: ", -1);printInt32(exit_status);upRollScreen();
        while(1){
            debug_log("parent");
            printString("test data: ", -1);printInt32(test_data);upRollScreen();
            sleep(200);
        }
    }

    while(1){}
}

void ASM_DEBUG_OUTPUT(){
    debug_log("ASM: DEBUG");
}

void shell(){
    printf("SHELL START\n");
    char cmd_buf[64];
    char cmd_len;
    while(1){
        write(STDOUT, "$ ", 2);
        cmd_len = read(STDIN, cmd_buf, 50);
        if(cmd_len == -1){
            printString("TTY read fail~~", -1);
            __asm__("hlt\n\t"::);
            while(1){}
        }
        cmd_buf[cmd_len] = '\0';
        printf("%s\n", cmd_buf);


    }

    error_log("SHELL EXIT");
    exit(0);
}

void Init(){
    sleep(2000);
    int fd_stdin = open("/dev_tty0", O_RDWR);
    int fd_stdout = open("/dev_tty0", O_RDWR);
    printf("STDIN   FD: %d\n", fd_stdin);
    printf("STDOUOT FD: %d\n", fd_stdout);
    printf("INIT START\n");
    if(!fork()){
        printf("SHELL FORK SUCCESS\n");
        shell();
    }
    else{
        // collect zombie child process and destroy them
        while(1){
            int child_exit_status;
            wait(&child_exit_status);
        }
    }
}

int main(){
    printString("main\n", -1);

    // init process state
    for(int i = 0; i < MAX_PROCESS_NUM; i++){
        pcb_table[i].state = PROCESS_EMPTY;
    }

    sys_create_process(tty_main, PRIVILEGE_KERNEL, 0);
    sys_create_process(hd_main, PRIVILEGE_KERNEL, 0);
    sys_create_process(fs_main, PRIVILEGE_KERNEL, 0);
    sys_create_process(mm_main, PRIVILEGE_KERNEL, 0);
    // sys_create_process(p1test, PRIVILEGE_USER, 0);
    // sys_create_process(p3test, PRIVILEGE_USER, 0);
    sys_create_process(Init, PRIVILEGE_USER, 0);
    // sys_create_process(fork_test, PRIVILEGE_USER, 0);
    
    current_process = pcb_table;
    
    switch_to_user_mode();

    while(1){}
}
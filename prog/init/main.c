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
#include "tar.h"

void sleep(int time){
    for(int i = 0; i < time; i++)
        for(int i = 0; i < 10000; i++)
            ;
}

void file_stat_test(){
    printf("file stat test\n");
    File_Stat file_stat;
    if(stat("/something.txt", &file_stat) == RESPONSE_SUCCESS){
        printf("file state: \n");
        printf("device: %d\n", file_stat.device);
        printf("inode_nr: %d\n", file_stat.inode_nr);
        printf("file_mode: %x\n", file_stat.file_mode);
        printf("start_sec: %d\n", file_stat.start_sec);
        printf("size: %d\n", file_stat.size);
    }
    else{
        printf("error: cannot get file state\n");
    }
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

void untar(char* tar_path){
    int inited_fd = open("/inited", O_CREATE | O_RDWR);
    if(inited_fd == -1){
        printf("untar was inited, skip\n");
        return;
    }
    else{
        close(inited_fd);
    }

    printf("untar file: %s\n", tar_path);
    int tar_fd = open(tar_path, O_RDWR);
    if(tar_fd == -1){
        printf("error: cannot open tar\n");
    }
    int file_fd;
    uint8_t buf[514];
    char name_buf[64];
    while(1){
        memset(buf, 0, sizeof(uint8_t) * 514);
        int head_len = read(tar_fd, buf, 512);
        printString("head fname: ", -1);printString((char*)buf, 15);printString("\n", -1);
        if(head_len == 0 || buf[0] == 0){
            break;
        }
        Tar_Header* th = (Tar_Header*)buf;
        char * cp = th->size;
		int f_len = 0;
		while (*cp){
			f_len = (f_len * 8) + (*cp - '0');
            cp++;
        }
        int real_flen = f_len;
        if(f_len % 512)
            f_len += (512 -  f_len % 512);

        memset(name_buf, 0, sizeof(char) * 64);
        name_buf[0] = '/';
        printf("FILE NAME: %s\n", th->name);
        strcpy(&name_buf[1], th->name);
        
        // while(1){}
        file_fd = open(name_buf, O_CREATE | O_RDWR);
        if(tar_fd == -1){
            printf("error: cannot open new file\n");
        }
        int copied_len = 0;

        printf("untar file: %s\nreal size:%d\nload size:%d\n", name_buf, real_flen, f_len);

        while(copied_len < f_len){
            int step_len = min((f_len - copied_len), 512);
            read(tar_fd, buf, 512);
            int wlen = write(file_fd, buf, step_len);
            printf("copied_len=%d, step_len=%d, f_len=%d, wlen=%d\n", copied_len, step_len, f_len, wlen);
            copied_len += step_len;
        }
        close(file_fd);

        printf("%s untar success\n\n", name_buf);
        // break;
        // while(1){}
    }
    printf("before close\n");
    close(tar_fd);
    printf("after close\n");
    

}

void shell(){
    printf("SHELL START\n");
    char cmd_buf[64];
    char cmd_len;
    char exec_path[MAX_FILEPATH_LEN];
    char* argv[20];
    char** argv_p;
    while(1){
        printf("$ ");
        memset(cmd_buf, 0, sizeof(char) * 64);
        cmd_len = read(STDIN, cmd_buf, 50);
        if(cmd_len == -1){
            printString("TTY read fail~~", -1);
            __asm__("hlt\n\t"::);
            while(1){}
        }
        cmd_buf[cmd_len] = '\0';

        // printf("cmd size=%d\n", cmd_len);
        for(int i = 0; i < cmd_len; i++)
            if(cmd_buf[i] == ' ')
                cmd_buf[i] = '\0';

        memset(exec_path, 0, sizeof(char) * MAX_FILEPATH_LEN);
        strcpy(exec_path, cmd_buf);
        memset(argv, 0, sizeof(char*) * 20);
        argv_p = argv;
        for(int i = 0; i < cmd_len - 1; i++){
            if(cmd_buf[i] == '\0' && cmd_buf[i + 1] != '\0'){
                *argv_p = &cmd_buf[i + 1];
                argv_p++;
            }
        }
        *argv_p = 0;

        int pid = fork();
        if(!pid){
            if(!execv(exec_path, argv)){
                printf("command not found\n");
                exit(0);
            }
        }
        int status;
        wait(&status);
    }

    error_log("SHELL EXIT");
    exit(0);
}

void file_test(){
    printf("file test\n");
    int fd = open("/something.txt", O_RDWR);
    if(fd == -1){
        printf("error: cannot open tar\n");
    }
    printf("open file ok\n");
    char* buf[20];
    read(fd, buf, 10);
    printf("file content: %s\n", buf);
    close(fd);
}

void dir_test(){
    int fd = open("/", O_RDWR);
    Dir_Entry de[20];
    read(fd, de, sizeof(Dir_Entry) * 20);
    for(int i = 0; i < 20; i++){
        printf("filename: %s\n", de[i].file_name);
    }
}

void Init(){
    // wait for fs service and mm service initialize
    int fs_service_init_ok = 0;
    int mm_service_init_ok = 0;
    Message msg;
    while(!(fs_service_init_ok && mm_service_init_ok)){
        memset(&msg, 0, sizeof(Message));
        ipc_recv(PID_ANY, &msg);
        // printString("got message from ", -1);printInt32(msg.src_pid);printString("\n", -1);
        switch(msg.src_pid){
            case PID_FS:
                fs_service_init_ok = 1;
                break;
            case PID_MM:
                mm_service_init_ok = 1;
                break;
            default:
                break;
        }
    }

    
    int fd_stdin = open("/dev_tty0", O_RDWR);
    int fd_stdout = open("/dev_tty0", O_RDWR);
    if(fd_stdin == -1){
        error_log("cannot open tty0 (stdin)");
        while(1){}
    }
    else if(fd_stdout == -1){
        error_log("cannot open tty0 (stdout)");
        while(1){}
    }
    printf("STDIN   FD: %d\n", fd_stdin);
    printf("STDOUOT FD: %d\n", fd_stdout);
    printf("INIT START\n");
    sleep(10);
    untar("/install.tar");
    printf("before shell\n");
    // dir_test();
    // file_test();
    // file_stat_test();
    if(!fork()){
        // while(1){
        //     printf("child\n");
        //     sleep(200);
        // }
        // execl("/echo", "aaa", "bbb");
        // sleep(100);
        shell();
        while(1){}
    }
    else{
        // printf("parent\n");
        // sleep(200);
        // while(1){
        //     printf("parent\n");
        //     sleep(200);
        // }
        // collect zombie child process and destroy them
        // printf("collect zombies...\n");
        while(1){
            int child_exit_status;
            wait(&child_exit_status);
        }
    }
}

void dummy_process(){
    while(1);
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
    // sys_create_process(dummy_process, PRIVILEGE_KERNEL, 0);
    // sys_create_process(fork_test, PRIVILEGE_USER, 0);
    
    current_process = pcb_table;
    
    switch_to_user_mode();

    while(1){}
}
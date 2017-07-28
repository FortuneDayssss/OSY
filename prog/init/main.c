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
        test();
        // printString("p1---   ", -1);
        // upRollScreen();
        // keyboard_read();
        //ttywrite("asdasdasd", -1);
        sleep(100);
        // sleep(100);
    }
}

void p2test(){
    uint64_t tempres;
    uint8_t tempbuf[20];
    while(1){
        // printInt32(tempbuf);
        // upRollScreen();
        tempres = getcharfromkeybuffer(tempbuf, 19);
        if(tempres > 0){
            printString(tempbuf, tempres);
        }
        // printInt32(tempres);
        //printString("p2---   ", -1);
        //system call "test"
        // __asm__(
            // "int    $0x80\n\t"
            // :
            // :"=a"(tick)
            // :"a"(__NR_printkeyboardbuffer)
        // );
        // upRollScreen();
        // printInt32((uint32_t)tick);
        // upRollScreen();
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

    current_process = pcb_table;

    switch_to_user_mode();

    while(1){}
}
#ifndef _PROCESS_H__
#define _PROCESS_H__
#include "type.h"
#include "protect.h"
#include "tty.h"
#include "ipc.h"
#include "fs.h"

#define MAX_PROCESS_NUM     10
#define STACK_SIZE          4*1024

#define PROCESS_RUNNING         0       //running
#define PROCESS_INTERRUPTED     1       //interruptable sleep
#define PROCESS_UNINTERRUPTABLE 2       //uninterruptable sleep
#define PROCESS_STOPPED         3       //stopped
#define PROCESS_DEAD            4       //zombie
#define PROCESS_EMPTY           5       //(for test) avaliable pcb
#define PROCESS_READY           6       //for test
#define PROCESS_WAITING         7       //wait for signal

#define IPC_FLAG_NONE           0x00    //unblock
#define IPC_FLAG_SENDING        0x01    //block because sending msg,                0001b
#define IPC_FLAG_RECEIVEING     0x02    //block because recving msg,                0010b
#define IPC_FLAG_WAITING        0x04    //block because waiting child exit,         0100b
#define IPC_FLAG_HANGING        0x08    //block because waiting parent clean up,    1000b

#define PID_ANY                 0xFFFFFFFF
#define PID_INT                 0xFFFFFFFE
#define PID_INVALID             0xFFFFFFFD
#define PID_TTY                 0
#define PID_HD                  1
#define PID_FS                  2
#define PID_MM                  3
#define PID_INIT                4

#define FILP_TABLE_SIZE         64

typedef struct PCB_struct{
    //--------------same as asm macro----------------
    uint8_t             stack0[STACK_SIZE];
    uint8_t             stack3[STACK_SIZE];
    uint32_t            esp;
    uint16_t            ldt_selector;
    Descriptor          ldts[LDT_SIZE];
    //--------------same as asm macro----------------
    uint32_t            pid;
    char                name[32];
    uint32_t            state;
    uint32_t            tick;
    TTY*                tty;
    
    uint32_t            has_int_message;
    uint32_t            pid_send_to;
    uint32_t            pid_recv_from;
    uint32_t            ipc_flag;
    Message*            message_ptr;
    struct PCB_struct*  message_queue;
    struct PCB_struct*  next_sender;

    uint32_t            parent;
    uint32_t            exit_status;

    File_Descriptor*    filp_table[FILP_TABLE_SIZE];
}PCB;

void schedule();                    //process.c
void switch_to_next_process();      //kernel.asm
uint32_t get_process_phy_mem(uint32_t pid, uint32_t addr);
uint32_t get_process_vir_mem(uint32_t pid, uint32_t phy_addr);
uint32_t sys_create_process(void (*startAddr), uint32_t privilege, uint32_t nr_tty);//process.c
uint32_t sys_ipc_send(uint32_t dst_pid, Message* msg_ptr);
uint32_t sys_ipc_recv(uint32_t src_pid, Message* msg_ptr);
uint32_t sys_ipc_int_send(uint32_t dst_pid);
#endif
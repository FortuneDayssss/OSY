#include "debug.h"
#include "type.h"
#include "print.h"
#include "global.h"
#include "process.h"


uint32_t debug_print_process_info(uint32_t pid){
    char p_name_desc[][64] = {	
            "(TTY)",
            "(HD)",
            "(FS)",
            "(MM)",
            "(INIT)",
            "(SHELL)"
    };
    char p_state_desc[][64] = {	
            "PROCESS_RUNNING",
            "PROCESS_INTERRUPTED",
            "PROCESS_UNINTERRUPTABLE",
            "PROCESS_STOPPED",
            "PROCESS_DEAD",
            "PROCESS_EMPTY",
            "PROCESS_READY",
            "PROCESS_WAITING"
    };
    char p_ipc_desc[][64] = {	
            "PROCESS_RUNNING",
            "PROCESS_INTERRUPTED",
            "PROCESS_UNINTERRUPTABLE",
            "PROCESS_STOPPED",
            "PROCESS_DEAD",
            "PROCESS_EMPTY",
            "PROCESS_READY",
            "PROCESS_WAITING"
    };
    
    
    // pid, name
    printString("process ", -1);printInt32(pid);
    if(pid <= 5)printString(p_name_desc[pid], -1);
    printString("\n", -1);

    // state
    printString("   state: ", -1);printString(p_state_desc[pcb_table[pid].state], -1);printString("\n", -1);

    // ipc flag
    if(pcb_table[pid].ipc_flag != IPC_FLAG_NONE){
        printString("   ipc_flag: ", -1);
        if(pcb_table[pid].ipc_flag & IPC_FLAG_SENDING)
            printString("SENDING   ", -1);
        if(pcb_table[pid].ipc_flag & IPC_FLAG_RECEIVEING)
            printString("RECEIVEING   ", -1);
        if(pcb_table[pid].ipc_flag & IPC_FLAG_WAITING)
            printString("WAITING   ", -1);
        if(pcb_table[pid].ipc_flag & IPC_FLAG_HANGING)
            printString("HANGING   ", -1);
        
        if(pcb_table[pid].ipc_flag & IPC_FLAG_SENDING)
            printString("\n    pid_send_to: ", -1);printInt32(pcb_table[pid].pid_send_to);
        
        if(pcb_table[pid].ipc_flag & IPC_FLAG_RECEIVEING)
            printString("\n    pid_recv_from: ", -1);printInt32(pcb_table[pid].pid_recv_from);
    }
    printString("\n", -1);

    // int message
    printString("   has_int_message: ", -1);
    if(pcb_table[pid].has_int_message)
        printString("true\n", -1);
    else
        printString("false\n", -1);

    // message queue
    if(pcb_table[pid].message_queue){
        printString("   message_queue: ", -1);
        PCB* p = pcb_table[pid].message_queue;
        while(p){
            printInt32(p - pcb_table);
            p = p->next_sender;
        }
    }

    // empty line
    printString("\n", -1);
}
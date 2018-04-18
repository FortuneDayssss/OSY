#include "process.h"
#include "global.h"
#include "string.h"
#include "protect.h"
#include "print.h"

void ipc_block(uint32_t pid, uint32_t ipc_flags){//just can use in kernel privilege
    __asm__("cli\n\t" ::);
    pcb_table[pid].ipc_flag |= ipc_flags;
    
    __asm__(
        "pushf\n\t"
        "pushl  %%cs\n\t"
        "pushl  $2f\n\t"
        "pusha\n\t"
        "pushl  %%ds\n\t"
        "pushl  %%es\n\t"
        "pushl  %%fs\n\t"
        "pushl  %%gs\n\t"
        "call   %1\n\t"         //schedule();
        "subl   $4, %%esp\n\t"  //switch_to_next_process做了平衡clock_handler的栈平衡, 设置32位空位模拟clock_handler
        "jmp    %0\n\t"         //switch_to_next_process
        "2:\n\t"
        "sti"
        :
        :"m"(switch_to_next_process), "m"(schedule)
    );

}

void ipc_unblock(uint32_t pid, uint32_t ipc_flags){
    __asm__("cli\n\t" ::);
    pcb_table[pid].ipc_flag &= ~ipc_flags;
    __asm__("sti\n\t" ::);
}

void schedule(){
    next_process = current_process;
    current_process->tick = 200;
    int next_process_index = current_process - pcb_table;

    for(int i = 0; i < MAX_PROCESS_NUM; i++){
        next_process_index = (next_process_index + 1) % MAX_PROCESS_NUM;
        if(pcb_table[next_process_index].ipc_flag == IPC_FLAG_NONE
            && (pcb_table[next_process_index].state == PROCESS_READY
            || pcb_table[next_process_index].state == PROCESS_STOPPED)){
            next_process = &pcb_table[next_process_index];
            if(current_process->state == PROCESS_RUNNING){
                current_process->state = PROCESS_STOPPED;
            }
            next_process->state = PROCESS_RUNNING;
            break;
        }
    }

    // if(current_process != next_process){
    //     printString("next process:", -1);
    //     printInt32(next_process - pcb_table);
    //     upRollScreen();
    //     printString("fs is receiveing?", -1);
    //     printInt32(pcb_table[2].ipc_flag);
    //     printString("fs is wait for:", -1);
    //     printInt32(pcb_table[2].pid_recv_from);
    //     upRollScreen();
    //     printString("init is receiveing?", -1);
    //     printInt32(pcb_table[4].ipc_flag);
    //     printString("init is wait for:", -1);
    //     printInt32(pcb_table[4].pid_recv_from);
    //     upRollScreen();
    //     printString("shell is receiveing?", -1);
    //     printInt32(pcb_table[5].ipc_flag);
    //     printString("shell is wait for:", -1);
    //     printInt32(pcb_table[5].pid_recv_from);
    //     upRollScreen();
    //     printString("shell has int message? ", -1);
    //     printInt32(pcb_table[5].has_int_message);
    //     upRollScreen();
    // }
}

uint32_t sys_create_process(void (*startAddr), uint32_t privilege, uint32_t nr_tty){
    int pid = -1;
    for(int i = 0; i < MAX_PROCESS_NUM; i++){
        if(pcb_table[i].state == PROCESS_EMPTY){
            pid = i;
            break;
        }
    }

    PCB* pcb = pcb_table + pid;
    
    if(pid != -1){
        uint32_t* stackPointer = (uint32_t*)(&pcb->stack0[STACK_SIZE - 4]);//stack0 top
        switch(privilege){
            case  PRIVILEGE_USER:
                *stackPointer = INDEX_LDT_MEMD << 3 | SA_TIL | SA_RPL_USER; //ss
                stackPointer--;
                *stackPointer = (uint32_t)(&pcb->stack3[STACK_SIZE - 4]); //stack3 top
                stackPointer--;
                *stackPointer = 0x1202; //IF = 1, IOPL = 1
                stackPointer--;
                *stackPointer = INDEX_LDT_MEMC << 3 | SA_TIL | SA_RPL_USER;
                stackPointer--;
                *stackPointer = (uint32_t)startAddr;
                stackPointer--;
                *stackPointer = 0; //eax
                stackPointer--;
                *stackPointer = 0; //ecx
                stackPointer--;
                *stackPointer = 0; //edx
                stackPointer--;
                *stackPointer = 0; //ebx
                stackPointer--;
                *stackPointer = (uint32_t)(&pcb->stack3[STACK_SIZE - 4]); //esp
                stackPointer--;
                *stackPointer = 0; //ebp
                stackPointer--;
                *stackPointer = 0; //esi
                stackPointer--;
                *stackPointer = 0; //edi
                stackPointer--;
                *stackPointer = INDEX_LDT_MEMD << 3 | SA_TIL | SA_RPL_USER; //ds
                stackPointer--;
                *stackPointer = INDEX_LDT_MEMD << 3 | SA_TIL | SA_RPL_USER; //es
                stackPointer--;
                *stackPointer = INDEX_LDT_MEMD << 3 | SA_TIL | SA_RPL_USER; //fs
                stackPointer--;
                *stackPointer = SELECTOR_VIDEO; //gs
                pcb->esp = (uint32_t)stackPointer;
                break;
            case PRIVILEGE_KERNEL:
                *stackPointer = INDEX_LDT_MEMD << 3 | SA_TIL | SA_RPL_KERNEL; //ss
                stackPointer--;
                *stackPointer = (uint32_t)(&pcb->stack0[STACK_SIZE - 4]); //stack3 top
                stackPointer--;
                *stackPointer = 0x1202; //IF = 1, IOPL = 1
                stackPointer--;
                *stackPointer = INDEX_LDT_MEMC << 3 | SA_TIL | SA_RPL_KERNEL;
                stackPointer--;
                *stackPointer = (uint32_t)startAddr;
                stackPointer--;
                *stackPointer = 0; //eax
                stackPointer--;
                *stackPointer = 0; //ecx
                stackPointer--;
                *stackPointer = 0; //edx
                stackPointer--;
                *stackPointer = 0; //ebx
                stackPointer--;
                *stackPointer = (uint32_t)(&pcb->stack0[STACK_SIZE - 4]); //esp
                stackPointer--;
                *stackPointer = 0; //ebp
                stackPointer--;
                *stackPointer = 0; //esi
                stackPointer--;
                *stackPointer = 0; //edi
                stackPointer--;
                *stackPointer = INDEX_LDT_MEMD << 3 | SA_TIL | SA_RPL_KERNEL; //ds
                stackPointer--;
                *stackPointer = INDEX_LDT_MEMD << 3 | SA_TIL | SA_RPL_KERNEL; //es
                stackPointer--;
                *stackPointer = INDEX_LDT_MEMD << 3 | SA_TIL | SA_RPL_KERNEL; //fs
                stackPointer--;
                *stackPointer = SELECTOR_VIDEO; //gs
                pcb->esp = (uint32_t)stackPointer;
                break;
            default:
                return -1;
                break;
        }
        
        pcb->ldts[INDEX_LDT_MEMC] = gdt[INDEX_MEMC_0];
        pcb->ldts[INDEX_LDT_MEMD] = gdt[INDEX_MEMD_0];
        if(privilege == PRIVILEGE_USER){
            pcb->ldts[INDEX_LDT_MEMC].attr1 = DA_C | PRIVILEGE_USER << 5;
            pcb->ldts[INDEX_LDT_MEMD].attr1 = DA_DRW | PRIVILEGE_USER << 5;
        }
        else if(privilege == PRIVILEGE_KERNEL){
            pcb->ldts[INDEX_LDT_MEMC].attr1 = DA_C | PRIVILEGE_KERNEL << 5;
            pcb->ldts[INDEX_LDT_MEMD].attr1 = DA_DRW | PRIVILEGE_KERNEL << 5;
        }
        else{
            return -1;
        }
        pcb->state = PROCESS_READY;
        pcb->tick = 20;
        pcb->tty = tty_table + nr_tty;

        pcb->has_int_message = 0;
        pcb->pid_send_to = 0;
        pcb->pid_recv_from = 0;
        pcb->ipc_flag = 0;
        pcb->message_ptr = 0;
        pcb->message_queue = 0;
        pcb->next_sender = 0;
    }


    return pid;
}

uint32_t sys_kill_process(uint32_t pid){
    pcb_table[pid].state = PROCESS_EMPTY;
}

uint32_t sys_ipc_send(uint32_t dst_pid, Message* msg_ptr){
    PCB* sender_pcb = current_process;
    uint32_t sender_pid = current_process - pcb_table;
    PCB* receiver_pcb = pcb_table + dst_pid;
    ((Message*)get_process_phy_mem(sender_pid, (uint32_t)msg_ptr))->src_pid = sender_pid;

    // // for debug 
    // if(sender_pid == 6){
    //     printString("pid: ", -1);printInt32(sender_pid);printString(" send to pid: ", -1);printInt32(dst_pid);upRollScreen();
    //     printString("type: ", -1);printInt32(((Message*)get_process_phy_mem(sender_pid, (uint32_t)msg_ptr))->type);upRollScreen();
    // }

    // receiver is wait for current process
    if(
        (receiver_pcb->ipc_flag & IPC_FLAG_RECEIVEING) && 
        ((receiver_pcb->pid_recv_from == sender_pid) || (receiver_pcb->pid_recv_from == PID_ANY))
    ){
        memcpy(
            (void*)get_process_phy_mem(dst_pid, (uint32_t)receiver_pcb->message_ptr),
            (void*)get_process_phy_mem(sender_pid, (uint32_t)msg_ptr),
            sizeof(Message)
        );
        receiver_pcb->message_ptr = 0;
        ipc_unblock(dst_pid, IPC_FLAG_RECEIVEING);
        // printInt32(sender_pid);printString(" SENDER UNBLOCK PID ", -1);printInt32(dst_pid);printString("\n", -1);
    }
    // receiver is not wait for current process
    else{
        // printString("not waiting for it and wait for", -1);printInt32(receiver_pcb->pid_recv_from);printString("\n", -1);
        sender_pcb->pid_send_to = dst_pid;
        sender_pcb->message_ptr = msg_ptr;
        sender_pcb->next_sender = 0;
        // current process is first sender
        if(!receiver_pcb->message_queue){
            receiver_pcb->message_queue = sender_pcb;
        }
        // there are senders in queue
        else{
            PCB* last_sender = receiver_pcb->message_queue;
            while(last_sender->next_sender)
                last_sender = last_sender->next_sender;
            last_sender->next_sender = sender_pcb;
        }
        ipc_block(sender_pid, IPC_FLAG_SENDING);
    }
    return 0;
}

uint32_t sys_ipc_recv(uint32_t src_pid, Message* msg_ptr){
    PCB* sender_pcb = 0;
    uint32_t sender_pid; // src_pid: PID_INT or PID_ANY or real_sender_pid
                         // sender_pid: real sender pid
    if(src_pid != PID_ANY && src_pid != PID_INT){
        sender_pcb = pcb_table + src_pid;
    }
    PCB* receiver_pcb = current_process;
    uint32_t receiver_pid = current_process - pcb_table;

    
    uint32_t get_msg_ok = 0;
    if(src_pid == PID_ANY || src_pid == PID_INT){
        // have interrupt message
        if(receiver_pcb->has_int_message){
            ((Message*)get_process_phy_mem(receiver_pid, (uint32_t)msg_ptr))->src_pid = PID_INT;
            ((Message*)get_process_phy_mem(receiver_pid, (uint32_t)msg_ptr))->type = MSG_INT;
            receiver_pcb->has_int_message = 0;
            get_msg_ok = 1;
        }
        // have message in queue and want get message from any process
        else if(src_pid == PID_ANY && receiver_pcb->message_queue){
            sender_pcb = receiver_pcb->message_queue;
            sender_pid = sender_pcb - pcb_table;
            receiver_pcb->message_queue = sender_pcb->next_sender;
            ((Message*)get_process_phy_mem(sender_pid, (uint32_t)(sender_pcb->message_ptr)))->src_pid = sender_pid;
            memcpy(
                (void*)get_process_phy_mem(receiver_pid, (uint32_t)msg_ptr),
                (void*)get_process_phy_mem(sender_pid, (uint32_t)(sender_pcb->message_ptr)),
                sizeof(Message)
            );
            sender_pcb->message_ptr = 0;
            ipc_unblock(sender_pcb - pcb_table, IPC_FLAG_SENDING);
            get_msg_ok = 1;
        }
    }
    else if((sender_pcb->ipc_flag & IPC_FLAG_SENDING) && (sender_pcb->pid_send_to == receiver_pid)){
        // find src msg in queue
        if(receiver_pcb->message_queue == sender_pcb){
            receiver_pcb->message_queue = sender_pcb->next_sender;
        }
        else{
            PCB* previous_sender = receiver_pcb->message_queue;
            while(previous_sender->next_sender != sender_pcb)
                previous_sender = previous_sender->next_sender;
            previous_sender->next_sender = sender_pcb->next_sender;
        }
        sender_pcb->next_sender = 0;
        sender_pid = sender_pcb - pcb_table;
        ((Message*)get_process_phy_mem(sender_pid, (uint32_t)(sender_pcb->message_ptr)))->src_pid = sender_pid;
        memcpy(
            (void*)get_process_phy_mem(receiver_pid, (uint32_t)msg_ptr),
            (void*)get_process_phy_mem(src_pid, (uint32_t)sender_pcb->message_ptr),
            sizeof(Message)
        );
        sender_pcb->message_ptr = 0;
        ipc_unblock(sender_pcb - pcb_table, IPC_FLAG_SENDING);
        get_msg_ok = 1;
    }
    
    // cant get message, block receiver
    if(!get_msg_ok){
        receiver_pcb->pid_recv_from = src_pid;
        receiver_pcb->message_ptr = msg_ptr;
        ipc_block(receiver_pid, IPC_FLAG_RECEIVEING);
    }
}

uint32_t sys_ipc_int_send(uint32_t dst_pid){
    PCB* receiver_pcb = pcb_table + dst_pid;
    if(
        (receiver_pcb->ipc_flag & IPC_FLAG_RECEIVEING) && 
        (receiver_pcb->pid_recv_from == PID_ANY || receiver_pcb->pid_recv_from == PID_INT)){
        receiver_pcb->has_int_message = 0;
        ((Message*)get_process_phy_mem(dst_pid, (uint32_t)receiver_pcb->message_ptr))->src_pid = PID_INT;
        ((Message*)get_process_phy_mem(dst_pid, (uint32_t)receiver_pcb->message_ptr))->type = MSG_INT;
        receiver_pcb->message_ptr = 0;
        ipc_unblock(dst_pid, IPC_FLAG_RECEIVEING);
    }
    else{
        receiver_pcb->has_int_message = 1;
    }
}

uint32_t get_process_phy_mem(uint32_t pid, uint32_t addr){
    if(pid <= PID_INIT) // service and init process don't have ldt offset
        return addr;
    Descriptor* ldt = &(pcb_table[pid].ldts[INDEX_LDT_MEMD]);
    uint32_t base = get_desc_base(ldt);
    return base + addr;
}

uint32_t get_process_vir_mem(uint32_t pid, uint32_t phy_addr){
    if(pid < PID_INIT) // service and init process don't have ldt offset
        return phy_addr;
    Descriptor* ldt = &(pcb_table[pid].ldts[INDEX_LDT_MEMD]);
    uint32_t base = get_desc_base(ldt);
    return phy_addr - base;
}
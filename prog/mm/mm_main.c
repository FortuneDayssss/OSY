#include "mm.h"
#include "ipc.h"
#include "message.h"
#include "type.h"
#include "global.h"
#include "process.h"
#include "string.h"
#include "print.h"

uint32_t do_fork(Message* msg);

uint32_t alloc_mem_for_proc(uint32_t pid, uint32_t mem_size);

void init_mm(){

}

uint8_t kernel_stack_backup[4 * 1024];

void mm_main(){

    init_mm();
    debug_log("mm init ok");

    Message msg;
    int parent_pid;
    int child_pid;

    while(1){
        sys_ipc_recv(PID_ANY, &msg);
        printString("mm got message from ", -1);printInt32(msg.src_pid);printString("\n", -1);
        printString("msg type: ", -1);printInt32(msg.type);printString("\n", -1);
        switch(msg.type){
            case MSG_MM_FORK:
                debug_log("get fork message");
                parent_pid = msg.src_pid;
                child_pid = do_fork(&msg);
                printString("child pid:", -1);printInt32(child_pid);printString("\n", -1);
                if(child_pid != -1){//success, awake child process before awake parent
                    Message child_ok_msg;
                    memcpy(kernel_stack_backup, pcb_table[parent_pid].stack0, 4 * 1024);
                    msg.mdata_response.pid = 0;
                    sys_ipc_send(child_pid, &msg);
                    sys_ipc_recv(child_pid, &child_ok_msg);
                    if(child_ok_msg.type != MSG_MM_FORK_CHILD_OK)
                        error_log("fork child ok msg error");
                }
                msg.mdata_response.pid = child_pid;
                memcpy(pcb_table[parent_pid].stack0, kernel_stack_backup, 4 * 1024);
                sys_ipc_send(parent_pid, &msg);
                break;
            default:
                break;
        }
    }
}


uint32_t do_fork(Message* msg){
    __asm__("cli\n\t"::);
    uint32_t parent_pid = msg->src_pid;
    
    // find an available pcb
    uint32_t child_pid;
    for(child_pid = 0; child_pid < MAX_PROCESS_NUM; child_pid++){
        if(pcb_table[child_pid].state == PROCESS_EMPTY)
            break;
    }
    if(child_pid >= MAX_PROCESS_NUM){
        return -1;
    }

    printString("child pid:", -1);printInt32(child_pid);printString("\n", -1);
    printString("parnet pid:", -1);printInt32(parent_pid);printString("\n", -1);

    // duplicate pcb
    uint32_t child_ldt_selector = pcb_table[child_pid].ldt_selector;
    // pcb_table[child_pid] = pcb_table[parent_pid];
    memcpy(&pcb_table[child_pid], &pcb_table[parent_pid], sizeof(PCB));
    pcb_table[child_pid].ldt_selector = child_ldt_selector;
    pcb_table[child_pid].parent = parent_pid;


    // copy memory segment (flat addressing mode)
    Descriptor* parent_ldt = &(pcb_table[parent_pid].ldts[INDEX_LDT_MEMC]);
    Descriptor* child_ldt = &(pcb_table[child_pid].ldts[INDEX_LDT_MEMC]);
    void* parent_base = (void*)get_desc_base(parent_ldt);
    uint32_t parent_limit = ((parent_ldt->limit_high_attr2 & 0xF) << 16) | (parent_ldt->limit_low);
    uint32_t parent_size = ((parent_limit + 1) * ((parent_ldt->limit_high_attr2 & (DA_LIMIT_4K >> 8)) ? 4096 : 1));
    uint32_t child_base = alloc_mem_for_proc(child_pid, parent_size);
    // memcpy((void*)child_base, (void*)parent_base, parent_size);
    printString("child base:", -1);printInt32(child_base);printString("\n", -1);
    printString("parent base:", -1);printInt32((uint32_t)parent_base);printString("\n", -1);
    printString("len:", -1);printInt32(PROCESS_IMAGE_MEM_DEFAULT);printString("\n", -1);
    memcpy((void*)child_base, (void*)parent_base, PROCESS_IMAGE_MEM_DEFAULT);

    // init ldt
    init_descriptor(
        &(pcb_table[child_pid].ldts[INDEX_LDT_MEMC]),
        child_base,
        (PROCESS_IMAGE_MEM_DEFAULT - 1) >> 12,
        DA_LIMIT_4K | DA_32 | DA_C | PRIVILEGE_USER << 5
    );
    init_descriptor(
        &(pcb_table[child_pid].ldts[INDEX_LDT_MEMD]),
        child_base,
        (PROCESS_IMAGE_MEM_DEFAULT - 1) >> 12,
        DA_LIMIT_4K | DA_32 | DA_DRW | PRIVILEGE_USER << 5
    );

    // fs fork ----todo----
    __asm__("sti\n\t"::);
    return child_pid;
}

uint32_t alloc_mem_for_proc(uint32_t pid, uint32_t mem_size){
    return PROCESS_MEM_BASE + pid * PROCESS_IMAGE_MEM_DEFAULT;
}
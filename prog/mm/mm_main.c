#include "mm.h"
#include "ipc.h"
#include "message.h"
#include "type.h"
#include "global.h"
#include "process.h"
#include "string.h"
#include "print.h"
#include "stdio.h"
#include "elf.h"

// buffer for elf chache, used in exec (todo: define as a const variable?)
uint8_t* mm_elf_buffer =        (uint8_t*)((uint32_t)0x00090000);   // buffer for elf
uint32_t mm_elf_buffer_len =    0x0000FBFF;                         // 64KByte

// buffer for args, used in exec (todo: define as a const variable?)
uint8_t* mm_args_buffer =       (uint8_t*)((uint32_t)0x0009FC00);
uint32_t mm_args_buffer_len =   0x000003FF;


uint32_t do_fork(Message* msg);

uint32_t do_exit(Message* msg);

uint32_t do_wait(Message* msg);

uint32_t do_exec(Message* msg);

uint32_t alloc_mem_for_proc(uint32_t pid, uint32_t mem_size);

void init_mm(){
    mm_elf_buffer =     (uint8_t*)((uint32_t)0x00090000);
    mm_elf_buffer_len = 0x0000ffff;
}

uint8_t kernel_stack_backup[4 * 1024];

void mm_main(){

    init_mm();

    Message msg;
    int parent_pid;
    int child_pid;
    int temp_src_pid;

    sys_ipc_send(PID_INIT, &msg);


    while(1){
        sys_ipc_recv(PID_ANY, &msg);
        // printString("mm got message from ", -1);printInt32(msg.src_pid);printString("\n", -1);
        // printString("msg type: ", -1);printInt32(msg.type);printString("\n", -1);
        switch(msg.type){
            case MSG_MM_FORK:
                debug_log("get fork message");
                parent_pid = msg.src_pid;
                child_pid = do_fork(&msg);
                // printString("child pid:", -1);printInt32(child_pid);printString("\n", -1);
                if(child_pid != -1){//success, awake child process before awake parent
                    Message child_ok_msg;
                    memcpy(kernel_stack_backup, pcb_table[parent_pid].stack0, 4 * 1024);
                    msg.mdata_response.pid = 0;
                    sys_ipc_send(child_pid, &msg);
                    sys_ipc_recv(child_pid, &child_ok_msg);
                    debug_log("child awake parent");
                    if(child_ok_msg.type != MSG_MM_FORK_CHILD_OK)
                        error_log("fork child ok msg error");
                }
                msg.mdata_response.pid = child_pid;
                memcpy(pcb_table[parent_pid].stack0, kernel_stack_backup, 4 * 1024);
                sys_ipc_send(parent_pid, &msg);
                debug_log("fork ok");
                break;
            case MSG_MM_EXIT:
                do_exit(&msg);
                break;
            case MSG_MM_WAIT:
                do_wait(&msg);
                break;
            case MSG_MM_EXEC:
                temp_src_pid = msg.src_pid;
                msg.mdata_response.status = do_exec(&msg);
                if(msg.mdata_response.status == RESPONSE_FAIL){
                    sys_ipc_send(temp_src_pid, &msg);
                }
                break;
            default:
                break;
        }
        memset(&msg, 0, sizeof(Message));
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

    // printString("child pid:", -1);printInt32(child_pid);printString("\n", -1);
    // printString("parnet pid:", -1);printInt32(parent_pid);printString("\n", -1);

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
    // printString("child base:", -1);printInt32(child_base);printString("\n", -1);
    // printString("parent base:", -1);printInt32((uint32_t)parent_base);printString("\n", -1);
    // printString("len:", -1);printInt32(PROCESS_IMAGE_MEM_DEFAULT);printString("\n", -1);
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
    Message msg_to_fs;
    msg_to_fs.type = MSG_FS_FORK_FD;
    msg_to_fs.mdata_fs_fork_fd.pid = child_pid;
    sys_ipc_send(PID_FS, &msg_to_fs);
    sys_ipc_recv(PID_FS, &msg_to_fs);

    __asm__("sti\n\t"::);
    return child_pid;
}

uint32_t alloc_mem_for_proc(uint32_t pid, uint32_t mem_size){
    return PROCESS_MEM_BASE + pid * PROCESS_IMAGE_MEM_DEFAULT;
}

void free_mem_for_proc(uint32_t pid){
    return;
}

void clean_up(PCB* pcb){
    Message msg;
    msg.type = MSG_RESPONSE;
    msg.mdata_response.pid = pcb - pcb_table;
    msg.mdata_response.status = pcb->exit_status;
    sys_ipc_send(pcb->parent, &msg);
    pcb->state = PROCESS_EMPTY;
    pcb->ipc_flag = IPC_FLAG_NONE;
}

uint32_t do_exit(Message* msg){
    uint32_t status = msg->mdata_mm_exit.status;
    uint32_t parent_pid = pcb_table[msg->src_pid].parent;
    
    // set exit status
    pcb_table[msg->src_pid].exit_status = status;

    // fs exit
    Message msg_to_fs;
    msg_to_fs.type = MSG_FS_EXIT;
    msg_to_fs.mdata_fs_exit.pid = msg->src_pid;
    sys_ipc_send(PID_FS, &msg_to_fs);
    sys_ipc_recv(PID_FS, &msg_to_fs);

    // free memory (dummy)
    free_mem_for_proc(msg->src_pid);

    // notify parent
    if(pcb_table[parent_pid].ipc_flag & IPC_FLAG_WAITING){ // parent is waiting
        pcb_table[parent_pid].ipc_flag &= (~IPC_FLAG_WAITING);
        clean_up(&pcb_table[msg->src_pid]);
    } 
    else{ // parent is not waiting
        pcb_table[msg->src_pid].ipc_flag |= IPC_FLAG_HANGING;
    }

    // if exited process have child, make INIT as their new parent
    for(int i = 0; i < MAX_PROCESS_NUM; i++){
        if(pcb_table[i].parent == msg->src_pid){
            pcb_table[i].parent = PID_INIT;
            if((pcb_table[PID_INIT].ipc_flag & IPC_FLAG_WAITING) && (pcb_table[i].ipc_flag & IPC_FLAG_HANGING)){
                pcb_table[PID_INIT].ipc_flag &= (~IPC_FLAG_WAITING);
                clean_up(&pcb_table[i]);
            }
        }
    }

    
}

uint32_t do_wait(Message* msg){
    uint32_t parent_pid = msg->src_pid;
    uint32_t children_counter = 0;
    for(int i = 0; i < MAX_PROCESS_NUM; i++){
        if(pcb_table[i].parent == parent_pid){
            children_counter++;
            if(pcb_table[i].ipc_flag & IPC_FLAG_HANGING){ // have child and child was exited, clean up
                clean_up(&pcb_table[i]);
                return 0;
            }
        }
    }

    if(children_counter){ // have child and child is running, set waiting flag
        pcb_table[parent_pid].ipc_flag |= IPC_FLAG_WAITING;
    }
    else{ // do not have any child, return pid = -1
        Message msg_response;
        msg_response.type = MSG_RESPONSE;
        msg_response.mdata_response.pid = -1;
        sys_ipc_send(parent_pid, &msg_response);
    }
}

uint32_t do_exec(Message* msg){
    uint32_t pid = msg->src_pid;
    char*       path_buf =          (char*)get_process_phy_mem(pid, msg->mdata_mm_exec.path_name);
    uint32_t    path_len =          msg->mdata_mm_exec.path_name_len;
    uint8_t*    msg_args_buf =      (uint8_t*)get_process_phy_mem(pid, msg->mdata_mm_exec.args_buf);
    uint32_t    msg_args_buf_len =  msg->mdata_mm_exec.args_buf_len;
    

    // copy path
    char path_name[MAX_FILEPATH_LEN];
    memcpy(path_name, path_buf, path_len);
    path_name[path_len] = '\0';

    // get file states (size)
    File_Stat file_stat;
    if(stat(path_name, &file_stat) == RESPONSE_FAIL){// fail
        error_log("exec error, cannot get file state");
        return RESPONSE_FAIL;
    }
    
    // load file to buffer
    int fd = open(path_name, O_RDWR);
    if(fd == -1){// fail
        error_log("exec error, cannot open file");
        return RESPONSE_FAIL;
    }
    read(fd, mm_elf_buffer, file_stat.size);
    close(fd);
    
    // load ELF
    Elf32_Ehdr* elf_header = (Elf32_Ehdr*)(mm_elf_buffer);
    for(int i = 0; i < elf_header->e_phnum; i++){
        Elf32_Phdr* p_header = (Elf32_Phdr*)(mm_elf_buffer + elf_header->e_phoff + (i * elf_header->e_phentsize));
        if(p_header->p_type == PT_LOAD){
            memcpy(
                (void*)get_process_phy_mem(pid, (uint32_t)p_header->p_vaddr), 
                (void*)(mm_elf_buffer + p_header->p_offset),
                p_header->p_filesz
            );
        }
    }

    // copy args
    uint8_t* user_args_p = (uint8_t*)get_process_phy_mem(pid, (uint32_t)PROCESS_USER_ARGS_BASE);
    memset(mm_args_buffer, 0, mm_args_buffer_len);
    memset(user_args_p, 0, msg_args_buf_len);
    memcpy(mm_args_buffer, msg_args_buf, msg_args_buf_len);
    memcpy(
        (void*)(user_args_p),
        (void*)(mm_args_buffer),
        msg_args_buf_len
    );

    // generate argv
    uint32_t user_argc = 1;
    uint32_t* user_argv = (uint32_t*)(user_args_p + msg_args_buf_len + 2);
    *user_argv = (uint32_t)user_args_p;// first arg
    user_argv++;

    for(int i = 0; i < min(msg_args_buf_len, mm_args_buffer_len) - 1; i++){
        if(*(user_args_p + i) == '\0' && *(user_args_p + i + 1) != '\0'){
            *user_argv = (uint32_t)get_process_vir_mem(pid, (uint32_t)(user_args_p + i + 1));
            user_argv++;
            user_argc++;
        }
    }
    user_argv = (uint32_t*)(user_args_p + msg_args_buf_len + 2);

    // kernel stack
    uint32_t* kernel_stack_p = (uint32_t*)(&pcb_table[pid].stack0[STACK_SIZE - 4]);
    *kernel_stack_p = INDEX_LDT_MEMD << 3 | SA_TIL | SA_RPL_USER;//ss
    kernel_stack_p--;
    *kernel_stack_p = PROCESS_USER_STACK_TOP; //stack3 top
    kernel_stack_p--;
    *kernel_stack_p = 0x1202; //IF = 1, IOPL = 1
    kernel_stack_p--;
    *kernel_stack_p = INDEX_LDT_MEMC << 3 | SA_TIL | SA_RPL_USER;
    kernel_stack_p--;
    *kernel_stack_p = (uint32_t)(elf_header->e_entry);
    kernel_stack_p--;
    *kernel_stack_p = (uint32_t)get_process_vir_mem(pid, (uint32_t)user_argv); //eax(argv)
    kernel_stack_p--;
    *kernel_stack_p = user_argc; //ecx(argc)
    kernel_stack_p--;
    *kernel_stack_p = 0; //edx
    kernel_stack_p--;
    *kernel_stack_p = 0; //ebx
    kernel_stack_p--;
    *kernel_stack_p = PROCESS_USER_STACK_TOP; //esp
    kernel_stack_p--;
    *kernel_stack_p = 0; //ebp
    kernel_stack_p--;
    *kernel_stack_p = 0; //esi
    kernel_stack_p--;
    *kernel_stack_p = 0; //edi
    kernel_stack_p--;
    *kernel_stack_p = INDEX_LDT_MEMD << 3 | SA_TIL | SA_RPL_USER; //ds
    kernel_stack_p--;
    *kernel_stack_p = INDEX_LDT_MEMD << 3 | SA_TIL | SA_RPL_USER; //es
    kernel_stack_p--;
    *kernel_stack_p = INDEX_LDT_MEMD << 3 | SA_TIL | SA_RPL_USER; //fs
    kernel_stack_p--;
    *kernel_stack_p = SELECTOR_VIDEO; //gs
    pcb_table[pid].esp = (uint32_t)kernel_stack_p;

    // init process state
    pcb_table[pid].state = PROCESS_READY;
    pcb_table[pid].tick = 20;
    // nr_tty = tty number before exec
    pcb_table[pid].has_int_message = 0;
    pcb_table[pid].pid_send_to = 0;
    pcb_table[pid].pid_recv_from = 0;
    pcb_table[pid].ipc_flag = IPC_FLAG_NONE;
    pcb_table[pid].message_ptr = 0;
    pcb_table[pid].message_queue = 0;
    pcb_table[pid].next_sender = 0;
    
    // debug_log("EXEC OK!");
    return RESPONSE_SUCCESS;
}

#include "tty.h"
#include "systemcall.h"
#include "type.h"
#include "print.h"
#include "global.h"
#include "keyboard.h"
#include "string.h"
#include "debug.h"

void switch_tty(int n){
    if (n >= NR_TTYS || n < 0)
        return;
    current_tty = &tty_table[n];
    screen_flush(&(current_tty->console));
}

void tty_init(TTY *pt){
    tty_buffer_init(&(pt->keyBuffer));
    tty_console_init(&(pt->console), pt - tty_table);
    pt->have_hooked_proc = 0;
}

//local parameters for keyboard
uint32_t code_with_E0 = 0;

void tty_read_loop(TTY *pt){
    uint32_t key;
    uint32_t shift;
    uint32_t ctrl;
    uint32_t alt;
    uint32_t make;
    uint32_t have_input;
    do{
        have_input = keyboard_process(pt, &key, &shift, &ctrl, &alt, &make);
        if(have_input && (key != 0) && make == 1 && key >= '1' && key <= '9' && ctrl){//ctrl+1~9, switch tty
            switch_tty(key - '1');
        }
        else if(have_input && (key != 0) && make == 1 && key >= '0' && key <= '9' && alt){ // for debug (alt + pid), output process information
            debug_print_process_info(key - '0');
        }
        else if(have_input && key ==ENTER && make == 1){//enter, roll up screen
            screen_roll_up(pt - tty_table);

            // press enter to finish process hooked reading
            if(pt->have_hooked_proc){// key => process buffer
                pt->have_hooked_proc = 0;
                Message msg;
                msg.type = MSG_TTY_READ_OK;
                msg.mdata_tty_read_ok.user_pid = pt->hooked_pid;
                msg.mdata_tty_read_ok.len = pt->copied_len;
                sys_ipc_send(PID_FS, &msg);
            }
        }
        else if(have_input && key != 0 && (!(key & FLAG_EXT)) && make == 1){//char output
            // process buffer is full, finish process hooked reading
            if(pt->have_hooked_proc){// key => process buffer
                pt->proc_buf[pt->copied_len] = key;
                pt->copied_len++;
                if(pt->copied_len >= pt->proc_buf_len){
                    pt->have_hooked_proc = 0;
                    Message msg;
                    msg.type = MSG_TTY_READ_OK;
                    msg.mdata_tty_read_ok.user_pid = pt->hooked_pid;
                    msg.mdata_tty_read_ok.len = pt->copied_len;
                    sys_ipc_send(PID_FS, &msg);
                }
            }
            // key => tty buffer
            *(pt->keyBuffer.head) = key;
            pt->keyBuffer.head++;
            if (pt->keyBuffer.head == pt->keyBuffer.buffer + TTY_BUFFER_SIZE)
                    pt->keyBuffer.head = pt->keyBuffer.buffer;
            pt->keyBuffer.count++;
            
        }
    }while(have_input);
}

void tty_write_loop(TTY *pt){
    while(pt->keyBuffer.count > 0){
        screen_out_char(pt - tty_table, *(pt->keyBuffer.tail));
        pt->keyBuffer.tail++;
        if (pt->keyBuffer.tail == pt->keyBuffer.buffer + TTY_BUFFER_SIZE)
            pt->keyBuffer.tail = pt->keyBuffer.buffer;
        pt->keyBuffer.count--;
    }
}

int tty_do_read(Message* msg){
    TTY* tty = &tty_table[msg->mdata_tty_read.nr_tty];
    if(tty->have_hooked_proc){
        // printString("already hooked with ", -1);printInt32(tty->hooked_pid);printString("\n", -1);
        return 0;
    }
    else{
        tty->have_hooked_proc = 1;
        tty->hooked_pid = msg->mdata_tty_read.user_pid;
        // printInt32(tty->hooked_pid);printString("hooked on tty~", -1);printString("\n", -1);
        tty->proc_buf = (uint8_t*)(get_process_phy_mem(msg->mdata_tty_read.user_pid, msg->mdata_tty_read.buf));
        tty->proc_buf_len = msg->mdata_tty_read.len;
        tty->copied_len = 0;
        return 1;
    }
}

int tty_do_write(Message* msg){
    TTY* tty = &tty_table[msg->mdata_tty_write.nr_tty];
    uint8_t* buf = (uint8_t*)(get_process_phy_mem(msg->mdata_tty_write.user_pid, msg->mdata_tty_write.buf));
    int size = msg->mdata_tty_write.len;
    int counter = 0;
    for (int i = 0; (size == -1 || i < size) && buf[i] != '\0' && tty->keyBuffer.count < TTY_BUFFER_SIZE; i++){
        *(tty->keyBuffer.head) = buf[i];
        tty->keyBuffer.head++;
        if (tty->keyBuffer.head == tty->keyBuffer.buffer + TTY_BUFFER_SIZE)
        tty->keyBuffer.head = tty->keyBuffer.buffer;
        tty->keyBuffer.count++;
        counter++;
    }
    return counter;
}

void tty_main(){

    //init tty
    for (int i = 0; i < NR_TTYS; i++)
        tty_init(&tty_table[i]);
    current_tty = tty_table;

    //tty loop
    Message msg;
    while (1){
        tty_read_loop(current_tty);
        tty_write_loop(current_tty);
        sys_ipc_recv(PID_ANY, &msg);
        uint32_t pid = msg.src_pid;
        switch(msg.type){
            case MSG_TTY_READ:
                // printString("TTY got read message from ", -1);printInt32(msg.src_pid);printString("\n", -1);
                msg.type = MSG_RESPONSE;
                msg.mdata_response.status = tty_do_read(&msg);
                sys_ipc_send(pid, &msg);
                break;
            case MSG_TTY_WRITE:
                // printString("TTY got write message from ", -1);printInt32(msg.src_pid);printString("\n", -1);
                msg.type = MSG_RESPONSE;
                msg.mdata_response.len = tty_do_write(&msg);
                sys_ipc_send(pid, &msg);
                break;
            case MSG_INT:
                break;
            default:
                break;
        }
        // memset(&msg, 0, sizeof(Message));
    }
}

//tty buffer
void tty_buffer_init(TTY_KeyBuffer *pb){
    pb->count = 0;
    pb->head = pb->buffer;
    pb->tail = pb->buffer;
}

//console
void tty_console_init(TTY_Console *pc, int nr_console){
    pc->graphMemoryBase = CONSOLE_GMEM_SIZE * nr_console;
    pc->dispAddr = pc->graphMemoryBase;
    pc->cursorAddr = pc->graphMemoryBase + 80*24*2;
}
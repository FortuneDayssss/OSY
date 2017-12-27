#include "type.h"
#include "systemcall.h"
#include "print.h"
#include "keyboard.h"
#include "global.h"

void sys_test(){
    upRollScreen();
    printString("test!\n", -1);
    // while(1){}
}

uint64_t get_ticks(){
    return sys_ticks;
}

//uint32_t test();
_system_call0(uint32_t,test)

//uint32_t times();
_system_call0(uint32_t,times)

//uint32_t ttywrite(uint8_t* buf, uint32_t buf_size);
_system_call2(uint32_t,ttywrite,uint8_t*,buf,uint32_t,size);

//uint32_t ipc_send(uint32_t dst_pid, Message* msg_ptr)
_system_call2(uint32_t,ipc_send,uint32_t,dst_pid,Message*,msg_ptr)

//uint32_t ipc_recv(uint32_t src_pid, Message* msg_ptr)
_system_call2(uint32_t,ipc_recv,uint32_t,src_pid,Message*,msg_ptr)


system_call_handler system_call_table[SYSTEM_CALL_NUM] = {
    (system_call_handler)sys_test,
    (system_call_handler)get_ticks,
    (system_call_handler)sys_tty_write,
    (system_call_handler)sys_ipc_send,
    (system_call_handler)sys_ipc_recv,
    0
};
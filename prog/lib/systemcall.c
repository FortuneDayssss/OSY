#include "type.h"
#include "systemcall.h"
#include "print.h"
#include "keyboard.h"
#include "global.h"

// //uint32_t test();
// _system_call0(uint32_t,test)

// //uint32_t times();
// _system_call0(uint32_t,times)

// //uint32_t ttywrite(uint8_t* buf, uint32_t buf_size);
// _system_call2(uint32_t,ttywrite,uint8_t*,buf,uint32_t,size);

//uint32_t ipc_send(uint32_t dst_pid, Message* msg_ptr)
_system_call2(uint32_t,ipc_send,uint32_t,dst_pid,Message*,msg_ptr)

//uint32_t ipc_recv(uint32_t src_pid, Message* msg_ptr)
_system_call2(uint32_t,ipc_recv,uint32_t,src_pid,Message*,msg_ptr)

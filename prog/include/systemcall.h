#ifndef _SYSTEM_CALL_H__
#define _SYSTEM_CALL_H__
#include "type.h"
#include "ipc.h"

#define __NR_test       0
#define __NR_times      1
#define __NR_ttywrite   2
#define __NR_ipc_send   3
#define __NR_ipc_recv   4

#define SYSTEM_CALL_NUM 10

#define _system_call0(type,name) \
type name(void){ \
    uint32_t __res; \
    __asm__ volatile( \
        "int    $0x80\n\t" \
        :"=a"(__res) \
        :"a"(__NR_##name) \
    ); \
    return __res; \
}

#define _system_call1(type,name,atype,a) \
type name(atype a){ \
    uint32_t __res; \
    __asm__ volatile( \
        "int    $0x80\n\t" \
        :"=a"(__res) \
        :"a"(__NR_##name),"b"((uint32_t)(a)) \
    ); \
    return __res; \
}

#define _system_call2(type,name,atype,a,btype,b) \
type name(atype a, btype b){ \
    uint32_t __res; \
    __asm__ volatile( \
        "int    $0x80\n\t" \
        :"=a"(__res) \
        :"a"(__NR_##name),"b"((uint32_t)(a)),"c"((uint32_t)(b)) \
    ); \
    return __res; \
}

uint32_t test();
uint32_t times();
uint32_t ttywrite(uint8_t* buf, uint32_t size);
uint32_t ipc_send(uint32_t dst_pid, Message* msg_ptr);
uint32_t ipc_recv(uint32_t src_pid, Message* msg_ptr);


#endif
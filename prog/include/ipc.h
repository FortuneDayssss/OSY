#ifndef _IPC_H__
#define _IPC_H__
#include "type.h"

#define MSG_INT         0xFFFFFFFE

typedef struct{
    uint32_t data1;
}Message1;

typedef struct{
    uint32_t src_pid;
    uint32_t type;
    union{
        Message1 msg1;
    };

}Message;

#endif
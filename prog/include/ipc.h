#ifndef _IPC_H__
#define _IPC_H__
#include "type.h"
#include "message.h"

enum msgtype{
    MSG_INT = 0xF1,

    MSG_RESPONSE,

    // hard disk drive
    MSG_HD_DEV_OPEN,
    MSG_HD_READ,
    MSG_HD_WRITE
};

typedef struct{
    uint32_t src_pid;
    uint32_t type;
    union{
        MData_Response      mdata_response;
        MData_HD_Read       mdata_hd_read;
        MData_HD_Write      mdata_hd_write;
    };

}Message;

#endif
#include "ipc.h"
#include "message.h"
#include "string.h"
#include "systemcall.h"
#include "process.h"

int close(int fd){
    Message msg;
    msg.type = MSG_FS_CLOSE;
    msg.mdata_fs_close.fd = (uint32_t)fd;
    ipc_send(PID_FS, &msg);
    ipc_recv(PID_FS, &msg);
    return 0;
}
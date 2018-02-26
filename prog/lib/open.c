#include "ipc.h"
#include "message.h"
#include "string.h"
#include "systemcall.h"
#include "process.h"

int open(const char* path_name, int flags){
    Message msg;
    msg.type = MSG_FS_OPEN;
    msg.mdata_fs_open.path_name = (uint32_t)path_name;
    msg.mdata_fs_open.path_name_len = strlen(path_name);
    msg.mdata_fs_open.flags = (uint32_t)flags;
    ipc_send(PID_FS, &msg);
    ipc_recv(PID_FS, &msg);
    return msg.mdata_response.fd;// return fd
}
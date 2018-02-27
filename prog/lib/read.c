#include "systemcall.h"
#include "ipc.h"
#include "message.h"
#include "type.h"
#include "process.h"

int read(int fd, void* buf, int count){
    Message msg;
    msg.type = MSG_FS_READ;
    msg.mdata_fs_read.fd = fd;
    msg.mdata_fs_read.buf = (uint32_t)buf;
    msg.mdata_fs_read.len = count;

    ipc_send(PID_FS, &msg);
    ipc_recv(PID_FS, &msg);
    return msg.mdata_response.len;
}

#include "systemcall.h"
#include "ipc.h"
#include "message.h"
#include "type.h"
#include "process.h"
#include "string.h"

int stat(const char* path, File_Stat* file_stat){
    Message msg;

	msg.type = MSG_FS_STAT;
	msg.mdata_fs_stat.path = (uint32_t)path;
    msg.mdata_fs_stat.path_len = (uint32_t)(strlen(path));
	msg.mdata_fs_stat.fs_stat_buf = (uint32_t)file_stat;

    ipc_send(PID_FS, &msg);
    ipc_recv(PID_FS, &msg);
    return msg.mdata_response.status;
}
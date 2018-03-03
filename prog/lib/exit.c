#include "ipc.h"
#include "message.h"
#include "systemcall.h"
#include "process.h"

void exit(int status){
    Message msg;
    msg.type = MSG_MM_EXIT;
    msg.mdata_mm_exit.status = (uint32_t)status;
    ipc_send(PID_MM, &msg);
    ipc_recv(PID_MM, &msg);
}

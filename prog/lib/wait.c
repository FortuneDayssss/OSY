#include "ipc.h"
#include "message.h"
#include "systemcall.h"
#include "process.h"

int wait(int* status){
    Message msg;
    msg.type = MSG_MM_WAIT;
    ipc_send(PID_MM, &msg);
    ipc_recv(PID_MM, &msg);

    *status = (int)msg.mdata_response.status;
    return msg.mdata_response.pid;
}
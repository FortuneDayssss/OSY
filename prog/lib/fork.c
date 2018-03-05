#include "ipc.h"
#include "message.h"
#include "string.h"
#include "systemcall.h"
#include "process.h"
#include "print.h"


int fork(){
    Message msg;
    msg.type = MSG_MM_FORK;
    // debug_log("before send fork msg");
    ipc_send(PID_MM, &msg);
    ipc_recv(PID_MM, &msg);
    // debug_log("after send fork msg");
    if(msg.mdata_response.pid == 0){ // awake child before awake parent
        msg.type = MSG_MM_FORK_CHILD_OK;
        ipc_send(PID_MM, &msg);
    }
    return msg.mdata_response.pid;  // -1: error, child: 0, other: child_pid
}
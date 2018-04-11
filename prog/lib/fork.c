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
    if(msg.mdata_response.is_parent == 0){ // awake child before awake parent
        // debug_log("user: child: stack ready, start to awake parent...");
        msg.type = MSG_MM_FORK_CHILD_OK;
        ipc_send(PID_MM, &msg);
        ipc_recv(msg.mdata_response.parent_pid, &msg);
        if(msg.type == MSG_MM_FORK_PARENT_OK){
            // debug_log("user: child fork ok!");
        }
        else
            error_log("fork fail in ipc between user processs");
        return 0;
    }
    else{
        msg.type = MSG_MM_FORK_PARENT_OK;
        ipc_send(msg.mdata_response.child_pid, &msg);
        // debug_log("user: parent fork ok!");
        return msg.mdata_response.child_pid;
    }
}
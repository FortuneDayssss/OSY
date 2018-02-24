#include "type.h"
#include "fs.h"
#include "print.h"
#include "ipc.h"
#include "message.h"
#include "process.h"

void init_fs();

void fs_main(){
    init_fs();

    Message msg;
    while(1){
        sys_ipc_recv(PID_ANY, &msg);
        switch(msg.type){
            // case MSG_HD_READ:
            //     hd_read(&msg);
            //     break;
            // case MSG_HD_WRITE:
            //     hd_write(&msg);
            //     break;
            default:
                break;
        }
    }
}

void init_fs(){


}
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
    MSG_HD_WRITE,
    MSG_FS_OPEN,
    MSG_FS_CLOSE,
    MSG_FS_READ,
    MSG_FS_WRITE,
    MSG_FS_UNLINK,
    MSG_FS_RESUME_PROC,
    MSG_FS_FORK_FD,
    MSG_FS_EXIT,
    MSG_TTY_READ,
    MSG_TTY_WRITE,
    MSG_TTY_READ_OK,
    MSG_PM_CREATE_PROCESS,
    MSG_PM_EXIT,
    MSG_MM_FORK,
    MSG_MM_FORK_CHILD_OK,
    MSG_MM_EXIT,
    MSG_MM_WAIT
};

typedef struct{
    uint32_t src_pid;
    uint32_t type;
    union{
        MData_Response          mdata_response;
        MData_HD_Read           mdata_hd_read;
        MData_HD_Write          mdata_hd_write;
        MData_FS_Open           mdata_fs_open;
        MData_FS_Close          mdata_fs_close;
        MData_FS_Read           mdata_fs_read;
        MData_FS_Write          mdata_fs_write;
        MData_FS_Fork_FD        mdata_fs_fork_fd;
        MData_FS_Exit           mdata_fs_exit;
        MData_TTY_Read          mdata_tty_read;
        MData_TTY_Write         mdata_tty_write;
        MData_TTY_Read_OK       mdata_tty_read_ok;
        MData_MM_Fork           mdata_mm_fork;
        MData_MM_Fork_Child_OK  mdata_mm_fork_child_ok;
        MData_MM_Exit           mdata_mm_exit;
        MData_MM_Wait           mdata_mm_wait;
    };

}Message;

#endif
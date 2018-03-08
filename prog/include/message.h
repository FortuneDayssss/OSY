#ifndef _MESSAGE_H__
#define _MESSAGE_H__

#include "type.h"
#include "stdio.h"

#define RESPONSE_SUCCESS 1
#define RESPONSE_FAIL 0

// data for response
typedef struct{
    uint32_t status;
    union{
        uint32_t fd;
        uint32_t len;
        uint32_t pid;
        uint32_t file_stat_buf;
    };
}MData_Response;


// data for request
typedef struct{
    uint32_t sector;
    uint32_t buf_addr;
    uint32_t len;
}MData_HD_Read;

typedef struct{
    uint32_t sector;
    uint32_t buf_addr;
    uint32_t len;
}MData_HD_Write;

typedef struct{
    uint32_t path_name;
    uint32_t path_name_len;
    uint32_t flags;
}MData_FS_Open;

typedef struct{
    uint32_t fd;
}MData_FS_Close;

typedef struct{
    uint32_t fd;
    uint32_t buf;
    uint32_t len;
}MData_FS_Read;

typedef struct{
    uint32_t fd;
    uint32_t buf;
    uint32_t len;
}MData_FS_Write;

typedef struct{
    uint32_t pid;
}MData_FS_Fork_FD;

typedef struct{
    uint32_t pid;
}MData_FS_Exit;

typedef struct{
    uint32_t path;
    uint32_t path_len;
    uint32_t fs_stat_buf;
}MData_FS_Stat;

typedef struct{
    uint32_t user_pid;
    uint32_t nr_tty;
    uint32_t buf;
    uint32_t len;
}MData_TTY_Read;

typedef struct{
    uint32_t user_pid;
    uint32_t nr_tty;
    uint32_t buf;
    uint32_t len;
}MData_TTY_Write;

typedef struct{
    uint32_t user_pid;
    uint32_t len;
}MData_TTY_Read_OK;

typedef struct{
    uint32_t dummy;
}MData_MM_Fork;

typedef struct{
    uint32_t dummy;
}MData_MM_Fork_Child_OK;

typedef struct{
    uint32_t status;
}MData_MM_Exit;

typedef struct{
    uint32_t dummy;
}MData_MM_Wait;

typedef struct{
    uint32_t path_name;
    uint32_t path_name_len;
    uint32_t args_buf;
    uint32_t args_buf_len;
}MData_MM_Exec;

#endif
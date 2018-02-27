#ifndef _MESSAGE_H__
#define _MESSAGE_H__

#define RESPONSE_SUCCESS 1
#define RESPONSE_FAIL 0
typedef struct{
    uint32_t status;
    union{
        uint32_t fd;
        uint32_t len;
    };
}MData_Response;

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

#endif
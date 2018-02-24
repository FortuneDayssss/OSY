#ifndef _MESSAGE_H__
#define _MESSAGE_H__

#define RESPONSE_SUCCESS 1
#define RESPONSE_FAIL 0
typedef struct{
    uint32_t status;
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

#endif
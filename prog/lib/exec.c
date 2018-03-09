#include "ipc.h"
#include "message.h"
#include "systemcall.h"
#include "process.h"
#include "string.h"

int exec(const char* path){
    char args_buf[MAX_FILEPATH_LEN];
    memset(args_buf, 0, sizeof(char) * MAX_FILEPATH_LEN);
    strcpy(args_buf, path);
    Message msg;
    msg.type = MSG_MM_EXEC;
    msg.mdata_mm_exec.path_name = (uint32_t)path;
    msg.mdata_mm_exec.path_name_len = strlen(path);
    msg.mdata_mm_exec.args_buf = (uint32_t)args_buf;
    msg.mdata_mm_exec.args_buf_len = strlen(args_buf);
    
    ipc_send(PID_MM, &msg);
    ipc_recv(PID_MM, &msg);
    return msg.mdata_response.status;
}

int execv(const char *path, char * argv[]){
	char **p = argv;
	char args_buf[128];
	int args_buf_len = 0;
    char* args_buf_p = args_buf;
    int step_len = 0;

    // copy path as argv[0]
    strcpy(args_buf_p, path);
    step_len = strlen(path);
    args_buf_p[step_len] = '\0';
    args_buf_p += step_len + 1;

    // copy other arg
    printf("path: %s\n", path);
    while(*p != 0){
        strcpy(args_buf_p, *p);
        step_len = strlen(*p);
        args_buf_p[step_len] = '\0';
        // printf("arg%d: %s\n", args_buf_p - args_buf, args_buf_p);
        args_buf_p += step_len + 1;
        p++;
    }

	args_buf_len = args_buf_p - args_buf;

	Message msg;
    msg.type = MSG_MM_EXEC;
    msg.mdata_mm_exec.path_name = (uint32_t)path;
    msg.mdata_mm_exec.path_name_len = strlen(path);
    msg.mdata_mm_exec.args_buf = (uint32_t)args_buf;
    msg.mdata_mm_exec.args_buf_len = args_buf_len;

	ipc_send(PID_MM, &msg);
    ipc_recv(PID_MM, &msg);

    return msg.mdata_response.status;
}

int execl(const char *path, const char *arg, ...){
	va_list parg = (va_list)(&arg);
	char **p = (char**)parg;
	return execv(path, p);
}
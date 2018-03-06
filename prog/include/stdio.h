#ifndef _STDIO_H__
#define _STDIO_H__

#include "type.h"

// file open mode
#define O_CREATE    1
#define O_RDWR      2

// standard fd number
#define STDIN       0
#define STDOUT      1

// string
#define	STR_DEFAULT_LEN	1024


int open(const char* path_name, int flags);

int read(int fd, void* buf, int count);

int write(int fd, void* buf, int count);

int close(int fd);

int fork();

void exit(int status);

int wait(int* status);

int printf(const char* fmt, ...);

int vsprintf(char* buf, const char* fmt, va_list args);

int sprintf(char *buf, const char *fmt, ...);

// todo: close, read, write, unlink, getpid ...

#endif

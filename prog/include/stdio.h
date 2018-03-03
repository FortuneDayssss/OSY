#ifndef _STDIO_H__
#define _STDIO_H__

// file open mode
#define O_CREATE    1
#define O_RDWR      2


int open(const char* path_name, int flags);

int read(int fd, void* buf, int count);

int write(int fd, void* buf, int count);

int close(int fd);

int fork();

void exit(int status);

int wait(int* status);

// todo: close, read, write, unlink, getpid ...

#endif

#ifndef _STDIO_H__
#define _STDIO_H__

// file open mode
#define O_CREATE    1
#define O_RDWR      2


int open(const char* path_name, int flags);
// todo: close, read, write, unlink, getpid ...

#endif

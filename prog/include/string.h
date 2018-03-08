#ifndef _STRING_H__
#define _STRING_H__

void*   memcpy(void* dst, void* src, int size);
void*   memset(void* dst, char ch, int size);
char*   strcpy(char* dst, const char* src);
int strlen(const char* s);
int strcmp(const char * cs,const char * ct);

#endif
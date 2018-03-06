#include "stdio.h"
#include "type.h"

int printf(const char* fmt, ...){
	char buf[STR_DEFAULT_LEN];

	va_list args = (va_list)(((uint32_t)(&fmt)) + 4);
	int len = vsprintf(buf, fmt, args);
	int wlen = write(STDOUT, buf, len);

	return wlen;
}

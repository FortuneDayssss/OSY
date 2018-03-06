#include "stdio.h"
#include "type.h"
#include "tty.h"

int printk(const char* fmt, ...){
	char buf[STR_DEFAULT_LEN];

	va_list args = (va_list)(((uint32_t)(&fmt)) + 4);
	int len = vsprintf(buf, fmt, args);
	int wlen = sys_tty_write(buf, len);

	return wlen;
}
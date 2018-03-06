#include "stdio.h"
#include "type.h"
#include "string.h"

char* int_to_str(int digit, int base, char** ps){
	int last_digit = digit % base;
	int except_last_digit = digit / base;
	if (except_last_digit) {
		int_to_str(except_last_digit, base, ps);
	}
	*(*ps) = (last_digit < 10) ? (last_digit + '0') : (last_digit - 10 + 'A');
    (*ps)++;
	return *ps;
}

int vsprintf(char* buf, const char* fmt, va_list args){
    char*   buf_p = buf;
    char    inner_buf[256];
    char*   inner_buf_p = inner_buf;
    va_list next_arg = args;
    for(; *fmt; fmt++){
        // normal character, just copy to buffer
        if(*fmt != '%'){
            *buf_p = *fmt;
            buf_p++;
            continue;
        }
        else if((*fmt == '%') && (*(fmt + 1) == '%')){
            *buf_p = *fmt;
            buf_p++;fmt++;
            continue;
        }

        fmt++;

        // save string formatted from args in inner_buf
        memset(inner_buf, 0, sizeof(char) * 256);
        inner_buf_p = inner_buf;
        switch(*fmt){
            case 'd':
                int_to_str(*((int*)next_arg), 10, &inner_buf_p);
                next_arg += 4;
                break;
            case 'x':
                int_to_str(*((int*)next_arg), 16, &inner_buf_p);
                next_arg += 4;
                break;
            case 'c':
                *inner_buf_p = *((char*)next_arg);
                next_arg += 4;
                break;
            case 's':
                strcpy(inner_buf, *((char**)next_arg));
                inner_buf_p += strlen(inner_buf);
                break;
            default:
                break;
        }

        // copy formatted string from inner_buf to buf
        char* copy_inner_p = inner_buf;
        while(copy_inner_p != inner_buf_p){
            *buf_p = *copy_inner_p;
            buf_p++;
            copy_inner_p++;
        }
    }

    // set buf tail = '\0'
    *buf_p = '\0';

    // return string length in buf
    return buf_p - buf; 
}

int sprintf(char *buf, const char *fmt, ...){
	va_list args = (va_list)((char*)(&fmt) + 4);
	return vsprintf(buf, fmt, args);
}
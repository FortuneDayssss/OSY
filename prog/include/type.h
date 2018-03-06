#ifndef _INTEGER_H__
#define _INTEGER_H__

typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
typedef unsigned int    uint32_t;
typedef unsigned long   uint64_t;

typedef char*           va_list;

typedef void            (*interrupt_handler)();
typedef void            (*irq_handler)();
typedef void            (*system_call_handler)();

#endif
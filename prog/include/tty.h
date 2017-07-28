#ifndef _TTY_H__
#define _TTY_H__
#include "type.h"

//TTY
#define NR_TTYS 8
#define TTY_BUFFER_SIZE 256

/* VGA */
#define	CRTC_ADDR_REG	    0x3D4	/* CRT Controller Registers - Addr Register */
#define	CRTC_DATA_REG	    0x3D5	/* CRT Controller Registers - Data Register */
#define	START_ADDR_H	    0xC	    /* reg index of video mem start addr (MSB) */
#define	START_ADDR_L	    0xD	    /* reg index of video mem start addr (LSB) */
#define	CURSOR_H	        0xE	    /* reg index of cursor position (MSB) */
#define	CURSOR_L	        0xF	    /* reg index of cursor position (LSB) */
#define	V_MEM_BASE	        0xB8000	/* base of color video memory */
#define	V_MEM_SIZE	        0x8000	/* 32K: B8000H -> BFFFFH */

#define SCREEN_SIZE         (80*25)
#define SCREEN_WIDTH        80
#define NR_CONSOLES         NR_TTYS
#define CONSOLE_GMEM_SIZE   0x1000  //4K

typedef struct{
    uint32_t graphMemoryBase;
    uint32_t dispAddr;
    uint32_t cursorAddr;
}TTY_Console;

void tty_console_init(TTY_Console* pc, int nr_console);

typedef struct{
    uint8_t*    head;
    uint8_t*    tail;
    uint32_t    count;
    uint8_t     buffer[TTY_BUFFER_SIZE];
}TTY_KeyBuffer;

void tty_buffer_init(TTY_KeyBuffer* pb);

typedef struct{
    TTY_KeyBuffer   keyBuffer;
    TTY_Console     console;
}TTY;

void tty_main();

uint32_t sys_tty_write(uint8_t* buf, uint32_t size);

#endif
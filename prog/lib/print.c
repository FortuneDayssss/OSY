#include "print.h"
#include "global.h"

void upRollScreen(){
	/**
	 * graphic memory offset = (80 * row + column) * 2
	 * 0 <= row <= 24
	 * 0 <= column <= 79
	 */
	for(int row = 0; row < 24; row++)
		for(int column = 0; column < 80; column++){
			int opAddr = (row * 80 + column) * 2;
			int srcAddr = ((row + 1) * 80 + column) * 2;
			__asm__(
				"movw	%%gs:(,%%eax),	%%dx\n\t"
				"movw	%%dx,		%%gs:(,%%ebx)\n\t"
				:
				:"a"(srcAddr), "b"(opAddr)
			);
		}
	for(int column = 0; column < 80; column++){
		int opAddr = (24 * 80 + column) * 2;
		__asm__(
			"movw	$0x0F20,	%%gs:(,%%ebx)\n\t"
			:
			:"b"(opAddr)
		);
	}
	dispPos = 0;
}

void printChar(char* cPtr){
	__asm__(
		"movl	$(80*24+0)*2,	%%ebx\n\t"
		"addl	%%ecx,		%%ebx\n\t"
		"addl	%%ecx,		%%ebx\n\t"
		"movb	$0x0F,		%%ah\n\t"
		"movb	(%%edx),	%%al\n\t"
		"movw	%%ax,		%%gs:(,%%ebx)\n\t"
		:
		:"d"(cPtr), "c"(dispPos)
	);
	if(dispPos >= 79)
		upRollScreen();
	else
		dispPos++;
}

void printString(char* str, int size){
	for(int i = 0; str[i] != '\0' && (size == -1 || i < size); i++){
		if(str[i] == '\n')
			upRollScreen();
		else
			printChar(str + i);
	}
}

void printInt32(uint32_t n){
	uint32_t temp;
	char c;
	for(int i = 7; i >= 0; i--){
		temp = n >> (i * 4);
		temp &= 15;
		c = (char)(temp);
		if(c <= 9){
			c += '0';
			printChar(&c);
		}
		else if (c < 16){
			c = 'A' + (c - 10);
			printChar(&c);
		}
	}

}

void printInt16(uint16_t n);

void printInt8(uint8_t n);
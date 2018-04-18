ENTRYPOINT	= 0x1000

ASM		= nasm
CC		= gcc
LD		= ld

ASMFLAGS	= -f elf
CFLAGS		= -I ../prog/include/ -c -fno-builtin -m32 -O0 -std=c99 -fno-stack-protector -fno-pie
LDFLAGS		= -Ttext $(ENTRYPOINT) -m elf_i386

OBJS 	= 	start.o \
			cat.o \
			echo.o \
			ls.o \

APPS	=	cat \
			echo \
			ls \

.PHONY: everything final clean realclean all

everything: $(OBJS) $(APPS)

all: realclean everything

final: all clean

tar:
	tar -cvf app.tar $(APPS) hello.txt

buildimg:
	dd if=app.tar of=../c.img bs=512 seek=32768 count=60 conv=notrunc

clean:
	rm -f $(OBJS)

realclean:
	rm -f $(OBJS) $(APPS) app.tar

cat: cat.o start.o osy_runtime.a
	$(LD) $(LDFLAGS) -o $@ $^

echo: echo.o start.o osy_runtime.a
	$(LD) $(LDFLAGS) -o $@ $^

ls: ls.o start.o osy_runtime.a
	$(LD) $(LDFLAGS) -o $@ $^

start.o: start.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

cat.o: cat.c
	$(CC) $(CFLAGS) -o $@ $<

echo.o: echo.c
	$(CC) $(CFLAGS) -o $@ $<

ls.o: ls.c
	$(CC) $(CFLAGS) -o $@ $<
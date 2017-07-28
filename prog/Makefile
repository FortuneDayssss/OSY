ENTRYPOINT	= 0x00010000
ENTRYOFFSET	= 0x00000000
SETUPSECNUM	= 3
SYSSECNUM	= 60

ASM		= nasm
CC		= gcc
LD		= ld
ASMBFLAGS	= -I boot/include/
ASMKFLAGS	= -I include/ -f elf
CFLAGS		= -I include/ -c -fno-builtin -m32 -O0 -std=c99 -fno-stack-protector -fno-pie
LDFLAGS		= -s -Ttext $(ENTRYPOINT) -m elf_i386

OSBOOT		= boot/bootsec.bin boot/setup.bin
OSKERNEL	= kernel.bin
OBJS		= kernel/kernel.o \
				kernel/clock.o \
				kernel/global.o \
				kernel/process.o \
				kernel/keyboard.o \
				kernel/systemcall.o \
				kernel/tty.o \
				init/main.o \
				init/init.o \
				init/protect.o \
				lib/string.o \
				lib/print.o 

.PHONY: everything final image clean realclean disasm all buildimg

.PHONY: everything final image clean realclean disasm all buildimg

everything: $(OSBOOT) $(OSKERNEL)

all: realclean everything

final: all clean

clean:
	rm -f $(OBJS)

realclean:
	rm -f $(OBJS) $(OSBOOT) $(OSKERNEL)

buildimg:
	dd if=boot/bootsec.bin of=../a.img bs=512 count=1 conv=notrunc
	dd if=boot/setup.bin of=../a.img bs=512 count=3 seek=1 conv=notrunc
	dd if=kernel.bin of=../a.img bs=512 count=$(SYSSECNUM) seek=`expr 1 + $(SETUPSECNUM)` conv=notrunc

boot/bootsec.bin: boot/bootsec.asm
	$(ASM) $(ASMBFLAGS) -o $@ $<

boot/setup.bin: boot/setup.asm boot/include/macro.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(OSKERNEL): $(OBJS)
	$(LD) $(LDFLAGS) -o $(OSKERNEL) $(OBJS)

kernel/kernel.o: kernel/kernel.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/clock.o: kernel/clock.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/global.o: kernel/global.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/process.o: kernel/process.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/keyboard.o: kernel/keyboard.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/systemcall.o: kernel/systemcall.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/tty.o: kernel/tty.c
	$(CC) $(CFLAGS) -o $@ $<

init/init.o: init/init.c
	$(CC) $(CFLAGS) -o $@ $<

init/main.o: init/main.c
	$(CC) $(CFLAGS) -o $@ $<

init/protect.o: init/protect.c
	$(CC) $(CFLAGS) -o $@ $<

lib/string.o: lib/string.c
	$(CC) $(CFLAGS) -o $@ $<

lib/print.o: lib/print.c
	$(CC) $(CFLAGS) -o $@ $<
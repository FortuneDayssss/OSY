ENTRYPOINT	= 0x00010000
ENTRYOFFSET	= 0x00000000

ASM		= nasm
CC		= gcc
LD		= ld
ASMBFLAGS	= -I boot/include/
ASMKFLAGS	= -I include/ -f elf
CFLAGS		= -I include/ -c -fno-builtin -m32 -O0 -std=c99
LDFLAGS		= -s -Ttext $(ENTRYPOINT) -m elf_i386

OSBOOT		= boot/bootsec.bin boot/setup.bin
OSKERNEL	= kernel.bin
OBJS		= kernel/kernel.o \
				kernel/global.o \
				init/main.o \
				init/init.o \
				lib/string.o

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
	dd if=kernel.bin of=../a.img bs=512 count=10 seek=4 conv=notrunc

boot/bootsec.bin: boot/bootsec.asm
	$(ASM) $(ASMBFLAGS) -o $@ $<

boot/setup.bin: boot/setup.asm boot/include/macro.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(OSKERNEL): $(OBJS)
	$(LD) $(LDFLAGS) -o $(OSKERNEL) $(OBJS)

kernel/kernel.o: kernel/kernel.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/global.o: kernel/global.c
	$(CC) $(CFLAGS) -o $@ $<

init/init.o: init/init.c
	$(CC) $(CFLAGS) -o $@ $<

init/main.o: init/main.c
	$(CC) $(CFLAGS) -o $@ $<

lib/string.o: lib/string.c
	$(CC) $(CFLAGS) -o $@ $<
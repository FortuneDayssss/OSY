ENTRYPOINT	= 0x00030400
ENTRYOFFSET	= 0x00000000

ASM		= nasm
CC		= gcc
LD		= ld
ASMBFLAGS	= -I boot/include/
ASMKFLAGS	= -I include/ -f elf
CFLAGS		= -I include/ -c -fno-builtin -m32 -O0 -fno-stack-protector
LDFLAGS		= -s -Ttext $(ENTRYPOINT) -m elf_i386

OSBOOT		= boot/bootsec.bin boot/loader.bin
OSKERNEL	= kernel.bin
OBJS		= kernel/kernel.o \
		  kernel/klib.o \
		  kernel/protect.o \
		  kernel/global.o \
		  init/main.o \
		  lib/string.o \
		  lib/print.o
		 

.PHONY: everything final image clean realclean disasm all buildimg

everything: $(OSBOOT) $(OSKERNEL)

all: realclean everything

final: all clean

image: final buildimg

clean:
	rm -f $(OBJS)

realclean:
	rm -f $(OBJS) $(OSBOOT) $(OSKERNEL)

buildimg:
	dd if=boot/bootsec.bin of=../a.img bs=512 count=1 conv=notrunc
	dd if=boot/loader.bin of=../a.img bs=512 count=5 seek=1 conv=notrunc
	dd if=kernel.bin of=../a.img bs=512 count=20 seek=5 conv=notrunc

boot/bootsec.bin: boot/bootsec.asm
	$(ASM) $(ASMBFLAGS) -o $@ $<

boot/loader.bin: boot/loader.asm boot/include/pm.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(OSKERNEL): $(OBJS)
	$(LD) $(LDFLAGS) -o $(OSKERNEL) $(OBJS)

kernel/kernel.o: kernel/kernel.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/klib.o: kernel/klib.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/protect.o: kernel/protect.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/global.o: kernel/global.c
	$(CC) $(CFLAGS) -o $@ $<

init/main.o: init/main.c
	$(CC) $(CFLAGS) -o $@ $<

lib/string.o: lib/string.c
	$(CC) $(CFLAGS) -o $@ $<

lib/print.o: lib/print.c
	$(CC) $(CFLAGS) -o $@ $<



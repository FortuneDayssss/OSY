ENTRYPOINT	= 0x00010000
ENTRYOFFSET	= 0x00000000
SETUPSECNUM	= 3
SYSSECNUM	= 100

ASM		= nasm
CC		= gcc
LD		= ld
ASMBFLAGS	= -I boot/include/
ASMKFLAGS	= -I include/ -f elf
CFLAGS		= -I include/ -c -fno-builtin -m32 -O0 -std=c99 -fno-stack-protector -fno-pie
LDFLAGS		= -s -Ttext $(ENTRYPOINT) -m elf_i386

OSBOOT		= boot/bootsec.bin boot/setup.bin boot/hdbootsec.bin
OSKERNEL	= kernel.bin
OBJS		= kernel/kernel.o \
				kernel/clock.o \
				kernel/global.o \
				kernel/hd.o \
				kernel/process.o \
				kernel/keyboard.o \
				kernel/tty.o \
				kernel/printk.o \
				kernel/debug.o \
				init/main.o \
				init/init.o \
				init/protect.o \
				lib/systemcall.o \
				lib/string.o \
				lib/print.o \
				lib/open.o \
				lib/read.o \
				lib/write.o \
				lib/close.o \
				lib/fork.o \
				lib/wait.o \
				lib/exit.o \
				lib/printf.o \
				lib/vsprintf.o \
				lib/stat.o \
				lib/exec.o \
				fs/fs_main.o \
				fs/open.o \
				fs/path.o \
				fs/read_write.o \
				mm/mm_main.o

.PHONY: everything final image clean realclean disasm all buildimg crt

.PHONY: everything final image clean realclean disasm all buildimg crt

everything: $(OSBOOT) $(OSKERNEL)

all: realclean everything

final: all clean

clean:
	rm -f $(OBJS)

realclean:
	rm -f $(OBJS) $(OSBOOT) $(OSKERNEL)
	rm -f lib/osy_runtime.a

buildimg:
	dd if=boot/bootsec.bin of=../a.img bs=512 count=1 conv=notrunc
	dd if=boot/setup.bin of=../a.img bs=512 count=3 seek=1 conv=notrunc
	dd if=kernel.bin of=../a.img bs=512 count=$(SYSSECNUM) seek=`expr 1 + $(SETUPSECNUM)` conv=notrunc
	dd if=boot/hdbootsec.bin of=../c.img bs=512 count=1 conv=notrunc

crt:
	ar rcs lib/osy_runtime.a lib/*.o

boot/bootsec.bin: boot/bootsec.asm
	$(ASM) $(ASMBFLAGS) -o $@ $<

boot/setup.bin: boot/setup.asm boot/include/macro.inc
	$(ASM) $(ASMBFLAGS) -o $@ $<

boot/hdbootsec.bin: boot/hdbootsec.asm
	$(ASM) $(ASMBFLAGS) -o $@ $<

$(OSKERNEL): $(OBJS)
	$(LD) $(LDFLAGS) -o $(OSKERNEL) $(OBJS)

kernel/kernel.o: kernel/kernel.asm
	$(ASM) $(ASMKFLAGS) -o $@ $<

kernel/clock.o: kernel/clock.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/global.o: kernel/global.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/hd.o: kernel/hd.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/process.o: kernel/process.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/keyboard.o: kernel/keyboard.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/tty.o: kernel/tty.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/printk.o: kernel/printk.c
	$(CC) $(CFLAGS) -o $@ $<

kernel/debug.o: kernel/debug.c
	$(CC) $(CFLAGS) -o $@ $<

init/init.o: init/init.c
	$(CC) $(CFLAGS) -o $@ $<

init/main.o: init/main.c
	$(CC) $(CFLAGS) -o $@ $<

init/protect.o: init/protect.c
	$(CC) $(CFLAGS) -o $@ $<

lib/systemcall.o: lib/systemcall.c
	$(CC) $(CFLAGS) -o $@ $<

lib/string.o: lib/string.c
	$(CC) $(CFLAGS) -o $@ $<

lib/open.o: lib/open.c
	$(CC) $(CFLAGS) -o $@ $<

lib/print.o: lib/print.c
	$(CC) $(CFLAGS) -o $@ $<

lib/read.o: lib/read.c
	$(CC) $(CFLAGS) -o $@ $<

lib/write.o: lib/write.c
	$(CC) $(CFLAGS) -o $@ $<

lib/close.o: lib/close.c
	$(CC) $(CFLAGS) -o $@ $<

lib/fork.o: lib/fork.c
	$(CC) $(CFLAGS) -o $@ $<

lib/wait.o: lib/wait.c
	$(CC) $(CFLAGS) -o $@ $<

lib/exit.o: lib/exit.c
	$(CC) $(CFLAGS) -o $@ $<

lib/printf.o: lib/printf.c
	$(CC) $(CFLAGS) -o $@ $<

lib/vsprintf.o: lib/vsprintf.c
	$(CC) $(CFLAGS) -o $@ $<

lib/stat.o: lib/stat.c
	$(CC) $(CFLAGS) -o $@ $<

lib/exec.o: lib/exec.c
	$(CC) $(CFLAGS) -o $@ $<

fs/fs_main.o: fs/fs_main.c
	$(CC) $(CFLAGS) -o $@ $<

fs/open.o: fs/open.c
	$(CC) $(CFLAGS) -o $@ $<

fs/path.o: fs/path.c
	$(CC) $(CFLAGS) -o $@ $<

fs/read_write.o: fs/read_write.c
	$(CC) $(CFLAGS) -o $@ $<

mm/mm_main.o: mm/mm_main.c
	$(CC) $(CFLAGS) -o $@ $<

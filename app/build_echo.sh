rm -f echo echo.o e.tar

gcc -m32 -I ../prog/include/ -c -fno-builtin -Wall -o echo.o echo.c

ld -Ttext 0x1000 -o echo echo.o start.o osy_runtime.a -m elf_i386

tar -cvf e.tar echo something.txt

rm -f echo echo.o e.tar

gcc -m32 -I ../prog/include/ -c -fno-builtin -Wall -O0 -std=c99 -fno-stack-protector -fno-pie -o echo.o echo.c
ld -Ttext 0x1000 -o echo echo.o start.o osy_runtime.a -m elf_i386

gcc -m32 -I ../prog/include/ -c -fno-builtin -Wall -O0 -std=c99 -fno-stack-protector -fno-pie -o ls.o ls.c
ld -Ttext 0x1000 -o ls ls.o start.o osy_runtime.a -m elf_i386

tar -cvf e.tar echo ls something.txt

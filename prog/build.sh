#!bin/bash

cd boot
nasm bootsec.asm -o bootsec.bin
nasm setup.asm -o setup.bin -I include/
cd ..
dd if=boot/bootsec.bin of=../a.img bs=512 count=1 conv=notrunc
dd if=boot/setup.bin of=../a.img bs=512 seek=1 count=3 conv=notrunc
dd if=sys-dummy.bin of=../a.img bs=512 seek=4 count=1 conv=notrunc

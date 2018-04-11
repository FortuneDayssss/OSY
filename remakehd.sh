
# generate runtime lib(osy_runtime.a)
cd prog
make all
make crt
mv lib/osy_runtime.a ../app

cd ../app
# build user mode apps(copied from app/build_app.sh)
rm -f echo echo.o e.tar
gcc -m32 -I ../prog/include/ -c -fno-builtin -Wall -O0 -std=c99 -fno-stack-protector -fno-pie -o echo.o echo.c
ld -Ttext 0x1000 -o echo echo.o start.o osy_runtime.a -m elf_i386
gcc -m32 -I ../prog/include/ -c -fno-builtin -Wall -O0 -std=c99 -fno-stack-protector -fno-pie -o ls.o ls.c
ld -Ttext 0x1000 -o ls ls.o start.o osy_runtime.a -m elf_i386
gcc -m32 -I ../prog/include/ -c -fno-builtin -Wall -O0 -std=c99 -fno-stack-protector -fno-pie -o cat.o cat.c
ld -Ttext 0x1000 -o cat cat.o start.o osy_runtime.a -m elf_i386
tar -cvf e.tar echo cat ls hello.txt
# copy end

# copy app tar into c.img(hard disk)
cd ..
dd if=app/e.tar of=c.img bs=512 seek=32768 count=60 conv=notrunc

# clean project
cd prog
make realclean

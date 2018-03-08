cd prog
make realclean
make
cd ..
ar rcs app/osy_runtime.a prog/lib/*.o
cd prog
make realclean

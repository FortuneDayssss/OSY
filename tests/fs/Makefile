
GCCFLAGS = -O0 -std=c99 -fno-stack-protector -fno-builtin -fprofile-arcs -ftest-coverage -g -fPIC -L/usr/local/lib -lcmocka
INC = -I ../../prog/include -I /usr/local/include/
EXECUTABLES = t_fs_main t_open t_path t_read_write

all: $(EXECUTABLES)

clean:
	rm -f *.gcov *.gcda *.gcno *.report
	rm -f $(EXECUTABLES)

t_fs_main: t_fs_main.c
	gcc -o $@ $(INC) $^ $(GCCFLAGS)
	./$@
	gcov $< >> $@.report

t_open: t_open.c
	gcc -o $@ $(INC) $^ $(GCCFLAGS)
	./$@
	gcov $< >> $@.report

t_path: t_path.c
	gcc -o $@ $(INC) $^ $(GCCFLAGS)
	./$@
	gcov $< >> $@.report

t_read_write: t_read_write.c
	gcc -o $@ $(INC) $^ $(GCCFLAGS)
	./$@
	gcov $< >> $@.report

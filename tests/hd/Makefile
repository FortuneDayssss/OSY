
GCCFLAGS = -O0 -std=c99 -fno-stack-protector -fno-builtin -fprofile-arcs -ftest-coverage -g -fPIC -L/usr/local/lib -lcmocka
INC = -I ../../prog/include -I /usr/local/include/
EXECUTABLES = t_hd

all: $(EXECUTABLES)

clean:
	rm -f *.gcov *.gcda *.gcno *.report
	rm -f $(EXECUTABLES)

t_hd: t_hd.c
	gcc -o $@ $(INC) $^ $(GCCFLAGS)
	./$@
	gcov $< >> $@.report

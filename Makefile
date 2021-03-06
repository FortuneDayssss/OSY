


.PHONY: all app os clean realclean run initrun refreshimg deletelog deleteimg deleteruntime projclean runhd default

default: os app realclean run

all: rebuildruntime os app realclean run

initrun: refreshimg all

app:
	(cd app; make all tar buildimg)

os:
	(cd prog; make final buildimg)

rebuildruntime:
	(rm -f app/osy_runtime.a; cd prog; make all crt; cp lib/osy_runtime.a ../app)
	(rm -f app/kernel.bin app/setup.bin; cd prog; make all crt; cp kernel.bin ../app; cp boot/setup.bin ../app)

clean:
	(cd app; make clean)
	(cd prog; make clean)

realclean:
	(cd app; make realclean)
	(cd prog; make realclean)

projclean: deletelog deleteimg deleteruntime realclean

refreshimg:
	rm -f a.img c.img
	bximage -q -mode=create -fd=1.44M a.img
	bximage -q -mode=create -hd=80M -imgmode=flat -q c.img

run:
	bochs -q

runhd:
	bochs -q -f bochsrc_hd

deletelog:
	rm -f bochsout.txt

deleteimg:
	rm -f a.img c.img

deleteruntime:
	rm -f app/osy_runtime.a

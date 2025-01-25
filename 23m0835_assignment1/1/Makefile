KDIR = /lib/modules/`uname -r`/build

all: test_builds kbuild

kbuild:
	make -C $(KDIR) M=`pwd`

lkm4_test.o:
	gcc -o lkm4_test.o test1.c

lkm5_test.o:
	gcc -o lkm5_test.o test2.c

test_builds: lkm4_test.o lkm5_test.o

clean:
	make -C $(KDIR) M=`pwd` clean

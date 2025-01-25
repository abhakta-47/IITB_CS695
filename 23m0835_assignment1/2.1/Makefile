obj-m += driver.o

all: driver program.o

driver:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

program.o: program.c
	gcc program.c -o program.o

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f *.o

spock: driver program.o
	insmod driver.ko
	./program.o
	rmmod driver

.PHONY: all driver program.o clean 
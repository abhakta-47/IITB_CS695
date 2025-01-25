obj-m += get_memstats.o

all: get_memstats

get_memstats:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules


clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f *.o


.PHONY: all clean 
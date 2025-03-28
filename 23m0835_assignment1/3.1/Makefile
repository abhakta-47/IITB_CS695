obj-m += get_pgfaults.o

all: get_pgfaults

get_pgfaults:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules


clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f *.o


.PHONY: all clean 
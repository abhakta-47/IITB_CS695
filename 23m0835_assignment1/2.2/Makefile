
TARGETS = control_station soldier

SOURCES = $(TARGETS:=.c)

all: $(TARGETS) driver2

obj-m += driver2.o
driver2:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

$(TARGETS): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $@.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -f $(TARGETS)


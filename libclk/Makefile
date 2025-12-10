-include ../Makefile.Include

libclk.so: nanosleep.o libevent.o libev.o
	$(C) -shared -o $@ $^

nanosleep.o \
libev.o \
: \
%.o: %.c
	$(C) -c -o $@ $<

libevent.o \
: \
%.o: %.c
	$(C) -c $(shell pkg-config --cflags libevent) -o $@ $<

.PHONY: clean

clean:
	rm -f *.so *.o *.d 2>/dev/null || true

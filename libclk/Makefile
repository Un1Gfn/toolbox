-include ../Makefile.Include

libclk.so: nanosleep.o timer.o
	$(C) -shared -o $@ $^

#libev.o
timer.o \
nanosleep.o \
: \
%.o: %.c
	$(C) -c -o $@ $<

#libevent.o \
#: \
#%.o: %.c
#	$(C) -c $(shell pkg-config --cflags libevent) -o $@ $<

.PHONY: clean

clean:
	rm -f *.so *.o *.d 2>/dev/null || true

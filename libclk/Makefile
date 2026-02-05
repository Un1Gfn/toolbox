-include ../Makefile.Include

# GSubprocess conflict
#O += libev.o

O += nanosleep.o

# race freeze bug
#O += timer.o

O += timerfd.o

# default target
libclk.so: $O
	$(C) -shared -o $@ $^

$O: %.o: %.c
	$(C) -c -o $@ $<

# compile with pkg-config
#libevent.o \
#: \
#%.o: %.c
#	$(C) -c $(shell pkg-config --cflags libevent) -o $@ $<

.PHONY: clean

clean:
	rm -f *.so *.o *.d 2>/dev/null || true

-include $(wildcard *.d)

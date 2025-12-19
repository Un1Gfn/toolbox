-include ../Makefile.Include

# compile without pkg-config
O := nanosleep.o timerfd.o libev.o

# race freeze bug
#O += timer.o

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

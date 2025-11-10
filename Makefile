MAKEFLAGS := -j30
C := gcc
CFLAGS := -std=gnu23 -g -Og -Wall -Wextra
FG:= $(shell pkg-config gtk4 --cflags)
LG:= $(shell pkg-config gtk4 --libs)

GUI := $(addsuffix .o,toolbox tab_base64)

toolbox: $(GUI)
	$(C) $(CFLAGS) -o $@ $? $(LG)

$(GUI): %.o: %.c
	$(C) -c $(CFLAGS) $(FG) -o $@ $?

#util.o lib.o: %.o:%.c
#	$(C) -c $(CFLAGS) -o $@ $?

.PHONY: clean
clean:
	@trash toolbox *.o 2>/dev/null || true

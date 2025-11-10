MAKEFLAGS := -j30
C := gcc
CFLAGS := -std=gnu23 -g -Og -Wall -Wextra -MMD -MP
FG:= $(shell pkg-config gtk4 --cflags)
L:= $(shell pkg-config --libs gtk4,openssl)

GUI := $(addsuffix .o,toolbox tab_base64)

toolbox: $(GUI)
	$(C) $(CFLAGS) $(GUI) $(L) -o $@

$(GUI): %.o: %.c
	$(C) -c $(CFLAGS) $(FG) -o $@ $<

#util.o lib.o: %.o:%.c
#	$(C) -c $(CFLAGS) -o $@ $?

.PHONY: clean
clean:
	@trash toolbox *.o *.d 2>/dev/null || true

#-include $(GUI:.o=.d)
-include $(wildcard *.d)


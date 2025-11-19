-include Makefile.Include

GUI := $(addsuffix .o,toolbox tab_base64 tab_ddc)

toolbox: $(GUI)
	$(C) $(CFLAGS) $(GUI) $(L) -o $@

$(GUI):
%.o: %.c tabs.h
	$(C) -c $(CFLAGS) $(FG) -o $@ $<

#util.o lib.o:
#%.o: %.c
#	$(C) -c $(CFLAGS) -o $@ $<

tabs.h: tabs.sh
	./$< >$@

.PHONY: clean
clean:
	@trash toolbox *.o *.d tabs.h 2>/dev/null || true

#-include $(GUI:.o=.d)
-include $(wildcard *.d)


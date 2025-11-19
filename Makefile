-include Makefile.Include

GUI := $(addsuffix .o,toolbox tab_base64 tab_ddc)

all: $(SO1)
	$(MAKE) toolbox

.PHONY: $(SO1)

$(foreach d, $(SO1), $(eval \
 $(d):; $(MAKE) -C $(d) $(d).so \
))

toolbox: $(GUI) $(SO2)
	$(C) $(CFLAGS) $(GUI) $(L) -o $@

$(GUI):
%.o: %.c tabs.h
	$(C) -c $(CFLAGS) $(FG) -o $@ $<

tabs.h: tabs.sh
	./$< >$@

.PHONY: clean purge

clean:
	@trash toolbox *.o *.d tabs.h 2>/dev/null || true
	@{ for d in $(SO1); do $(MAKE) -C $$d clean || true; done; } 2>/dev/null 1>/dev/null

purge:
	$(MAKE) clean
	@{ for i in $(SO1); do $(MAKE) -C $$i purge || true; done; } 2>/dev/null 1>/dev/null

#-include $(GUI:.o=.d)
-include $(wildcard *.d)


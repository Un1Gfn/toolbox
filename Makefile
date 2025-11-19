-include Makefile.Include

# alphabetical orer
GUI := $(addsuffix .o, \
 toolbox \
 tab_base64 \
 tab_ddc \
 tab_env \
 tab_welcome \
)

UTIL := $(addsuffix .o, \
 util \
)

OBJ := $(GUI) $(UTIL)

all: $(SO1)
	@$(MAKE) toolbox

.PHONY: $(SO1)

$(foreach d, $(SO1), $(eval \
 $(d):; $(MAKE) -C $(d) $(d).so \
))

toolbox: $(OBJ) $(SO2)
	$(C) $(OBJ) $(L) -o $@

$(GUI):
%.o: %.c tabs.h
	$(C) -c $(FG) -o $@ $<

$(UTIL):
%.o: %.c
	$(C) -c -o $@ $<

tab_welcome.c: tab_welcome.sh
	./$< >$@

tabs.h: tabs.sh
	./$< >$@

.PHONY: clean purge

clean:
	@trash toolbox *.o *.d tabs.h 2>/dev/null || true
	@{ for d in $(SO1); do $(MAKE) -C $$d clean || true; done; } 2>/dev/null 1>/dev/null

purge:
	$(MAKE) clean
	@{ for i in $(SO1); do $(MAKE) -C $$i purge || true; done; } 2>/dev/null 1>/dev/null

.PHONY: run_debug debug

run_debug: all
	env G_MESSAGES_DEBUG="$(G_LOG_DOMAIN) tab_base64" ./toolbox

run: all
	./toolbox

#-include $(GUI:.o=.d)
-include $(wildcard *.d)


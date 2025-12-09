-include Makefile.Include

# alphabetical orer
OBJ := $(addsuffix .o, \
 tab_base64 \
 tab_clk \
 tab_ddc \
 tab_env \
 tab_pdf \
 tab_welcome \
 toolbox \
 util \
)

all: $(SO1)
	@$(MAKE) toolbox

.PHONY: $(SO1)

$(foreach d, $(SO1), $(eval \
 $(d):; $(MAKE) -C $(d) $(d).so \
))

toolbox: $(OBJ) $(SO2)
	$(C) $(OBJ) $(shell pkg-config --libs gtk4,openssl,poppler-glib) $(SO3) -o $@

# papers-view-4.0 = crash
# evince-view-3.0 = gtk3 conflict with gtk4
# mupdf
tab_pdf.o \
: \
%.o: %.c tabs.h
	$(C) -c $(shell pkg-config --cflags gtk4,poppler-glib) -o $@ $<

tab_base64.o \
tab_clk.o \
tab_ddc.o \
tab_env.o \
tab_welcome.o \
toolbox.o \
util.o \
: \
%.o: %.c tabs.h
	$(C) -c $(shell pkg-config --cflags gtk4) -o $@ $<

#_.o \
#: \
#%.o: %.c
#	$(C) -c -o $@ $<

tabs.h \
tab_welcome.c \
: \
%: %.sh
	./$< >$@

.PHONY: clean purge

clean:
	@rm -f toolbox tab_welcome.c *.o *.d tabs.h 2>/dev/null || true
	@{ for d in $(SO1); do $(MAKE) -C $$d clean || true; done; } 2>/dev/null 1>/dev/null

purge:
	$(MAKE) clean
	@{ for i in $(SO1); do $(MAKE) -C $$i purge || true; done; } 2>/dev/null 1>/dev/null

.PHONY: run_debug debug

run_gdb: all
	gdb --args ./toolbox $(A)

run_debug: all
	env G_MESSAGES_DEBUG="$(DOMAIN)" ./toolbox $(A)
	@#env G_MESSAGES_DEBUG="all" ./toolbox

run: all
	./toolbox $(A)

#-include $(GUI:.o=.d)
-include $(wildcard *.d)


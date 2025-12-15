libcangjie5.so: table.o
	$(C) -shared -o $@ $^

table.o: %.o: %.c table.h
	$(C) -c -o $@ $<

table.c: gen.sh cangjie5.txt
	./gen.sh <cangjie5.txt

cangjie5.txt:
	wget https://wget.la/https://github.com/fcitx/fcitx5-table-extra/blob/0b3e04256b5b49c72a04e88da80bf34e5bba4b3f/tables/cangjie5.txt
	echo "7c3fb9626bcbbf750806506def6dc18796ccfa20c208332dd3c572ceb0abcc97  cangjie5.txt" | sha256sum -c -

.PHONY: clean purge
clean:
	rm -f *.so *.o *.d 2>/dev/null || true

purge:
	$(MAKE) clean
	rm -f table.c cangjie5.txt 2>/dev/null || true

-include ../Makefile.Include

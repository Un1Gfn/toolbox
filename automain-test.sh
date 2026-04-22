#!/bin/bash

source /automain.rc

export CFLAGS="$(pkg-config --cflags glib-2.0) -fsanitize=address -save-temps"
export LDLIBS="$(pkg-config --libs glib-2.0)"
p automain-test.c || exit 1

e=0
i=0
while [ 127 != "$e" ]; do
	echo mmm_$i
	"$_"
	e="$?"
	echo "=> $e"
	echo
	((i++))
done

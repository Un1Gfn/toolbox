#!/bin/bash -eu

S="$(cat cangjie5.txt | grep "^[a-z]" | cut -d " " -f 2-)"

L="$(echo "$S" | wc -l)"
M="$(echo "$S" | wc -m)"
[ "$M" = "$((L*2))" ]

A=($(echo "$S" | sed "s/^/'/g"))

N="$(printf "%d\n" "${A[@]}")"

echo "$N" | sort -n

printf "\nok\n\n"

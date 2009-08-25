#!/bin/bash

for i in $(ls *.c|sed -e 's/\.c/.o/g');
do
    s=${i%%.o};
    t=$s.mk;
    c=$s.c;
    printf "%s:\n\t\$(CC) \$(CFLAGS) -c %s\n" $i $c >$t; 
done

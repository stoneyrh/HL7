top_dir=.
top_srcdir=.
base=$(shell pwd)
objects=$(shell ls *.c | sed -re 's/\.c/.o/g')

CC=gcc

.PHONY: clean

clean:
	rm -f $(objects)

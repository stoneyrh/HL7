top_dir=.
top_srcdir=.
CFLAGS=-I../include -Iinclude -Wall -O2 -DDEBUG -g3 -dA
LDFLAGS=-L../ -lhl7c -L../

objects=$(shell ls *.c | sed -re 's/\.c/.o/g')
target=runtests

file=../data/adt_a04_13885_20090811203018
host=localhost
port=2700

.PHONY: clean

all: $(objects) $(target)

$(target): $(objects)
	@echo [CC] $?
	$(CC) $(CFLAGS) $(LDFLAGS) $? -o $@

test: $(target)
	./$(target) $(host) $(port) $(file)

strace: $(target)
	@strace ./$(target) $(host) $(port) $(file)

ltrace: $(target)
	@ltrace ./$(target) $(host) $(port) $(file)

mem: $(target) 
	@echo [MEMCHECK] $(target)
	@valgrind --leak-check=full -v ./$(target) $(host) $(port) $(file)

tags:
	ln -s ../tags
clean:
	-rm -f $(objects) $(target) core

include $(wildcard *.mk)

top_dir=.
top_srcdir=.
CFLAGS=-I../include -Iinclude -Wall -O2 -DDEBUG -g3 -dA
LDFLAGS=-L../ -lhl7c -L../
LD_LIBRARY_PATH=../

objects=$(shell ls *.c | sed -re 's/\.c/.o/g')
target=runtests

file=../data/adt_a04_13885_20090811203018
host=localhost
port=2700

.EXPORT_ALL_VARIABLES: environ
.PHONY: clean environ

all: clean environ $(objects) $(target)

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

environ:
	@echo fixing environ - make sure hl7lib has been built before running these tests!

tags:
	ln -s ../tags
clean:
	-rm -f $(objects) $(target) core

include $(wildcard *.mk)

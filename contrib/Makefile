top_dir=.
top_srcdir=.
CFLAGS=-I../include -Wall -O2 -DDEBUG -g3 -dA
LDFLAGS=-L../ -lhl7c -L../

objects=$(shell ls *.c | sed -re 's/\.c/.o/g')
target=server

.PHONY: clean

all: $(objects) $(target)

$(target): $(objects)
	@echo [CC] $?
	$(CC) $(CFLAGS) $(LDFLAGS) $? -o $@

clean:
	-rm -f $(objects) $(target) core

include $(wildcard *.mk)

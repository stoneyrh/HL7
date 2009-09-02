top_dir=.
top_srcdir=.
base=$(shell pwd)
data=$(base)/data
src=$(top_dir)/src
srcs=$(wildcard src/*.c)
objects=$(shell ls src/*.c | sed -re 's/\.c/.o/g')

target=libhl7c.so

CFLAGS=-Iinclude -Wall -O2 -D_GNU_SOURCE -DDEBUG -g3 -dA
LDFLAGS=-Wall --shared
CC=gcc

.PHONY: clean

all: clean $(objects) $(target)

$(target): $(objects)
	@echo [$(target)]
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(target) $(objects)

install:
	cp $(target) /usr/local/lib/
	ldconfig
clean:
	@-rm -f core src/*.o $(target)

tags:
	@ctags -a -R --recurse=yes --c++-kinds=+p --fields=+ifaS --extra=+q .

include $(wildcard src/*.mk)

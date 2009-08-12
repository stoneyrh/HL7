CFLAGS=-Iinclude -Wall -O2 -DDEBUG -g3 -dA -c
LDFLAGS=
CC=gcc
LD=gcc
SRCS= net.c array.c scan.c main.c
OBJS= net.o array.o scan.o main.o
CLIENT=hl7client
SERVER=debug_server

# For debugging
TFILE=messages/adt_a04_13885_20090811203018
THOST=localhost
TPORT=2701

.PHONY: clean

all: $(CLIENT) $(SERVER)

$(CLIENT): $(OBJS)
	@echo [LD] $(OBJS)
	@$(LD) -o $@ $(LDFLAGS) $(OBJS)

main.o: main.c
	@echo [CC] $?
	@$(CC) $(CFLAGS) $? 

scan.o: scan.c
	@echo [CC] $?
	@$(CC) $(CFLAGS) $?

array.o: array.c
	@echo [CC] $?
	@$(CC) $(CFLAGS) $? 

net.o: net.c
	@echo [CC] $?
	@$(CC) $(CFLAGS) $?

test: $(CLIENT) $(SERVER)
	./$(CLIENT) $(THOST) $(TPORT) $(TFILE)

strace: $(CLIENT) $(SERVER)
	@strace ./$(CLIENT) $(THOST) $(TPORT) $(TFILE)

ltrace: $(CLIENT) $(SERVER)
	@ltrace ./$(CLIENT) $(THOST) $(TPORT) $(TFILE)

$(SERVER): debug_server.c
	@echo [CC] $?
	@$(CC) $? -o $@

mem: $(CLIENT) $(SERVER)
	@echo [MEMCHECK] $(CLIENT)
	@valgrind --leak-check=full -v ./$(CLIENT) $(THOST) $(TPORT) $(TFILE)

clean:
	@-rm -f core *.o $(CLIENT) $(SERVER) tags *.log

tags:
	@ctags -a -R --recurse=yes --c++-kinds=+p --fields=+ifaS --extra=+q .


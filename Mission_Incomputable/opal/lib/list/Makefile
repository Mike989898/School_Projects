# Makefile for 'list' module

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb
PROG = listtest
OBJS = listtest.o list.o
LIBS =

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

listtest.o: list.h
list.o: list.h ../memory/memory.h

clean:
	rm -f *~
	rm -f *.o
	rm -f $(PROG)

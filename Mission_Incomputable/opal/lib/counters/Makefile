# Makefile for 'bag' module
#
# John Kotz, April 2016

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb
PROG = counterstest
OBJS = counterstest.o counters.o
LIBS =

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

counterstest.o: counters.h
counters.o: counters.h ../memory/memory.h

clean:
	rm -f *~
	rm -f *.o
	rm -f $(PROG)

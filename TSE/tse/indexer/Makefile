# Makefile for 'indexer' module
#
# Michael Kilgore, April 2016

PROG = indexer
OBJS = indexer.o
LIBS = -lcurl
LLIBS = ../lib/cs50ds.a ../common/common.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb
CC = gcc
MAKE = make

# build the indexer
$(PROG): $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

indextest: indextest.o $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -f *.o
	rm -f $(PROG)
	rm -f indextest

# Makefile for 'indexer' module
#
# Michael Kilgore and Akire Murphy, April 2016

PROG = querier 
OBJS = querier.o 
LIBS = 
LLIBS = ../lib/cs50ds.a ../common/common.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb
FUZZFLAGS = -Wall -pedantic -std=gnu11 -ggdb
CC = gcc
MAKE = make

# build the indexer
$(PROG): $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

fuzzquery: fuzzquery.o $(LLIBS)
	$(CC) $(FUZZFLAGS) $^ $(LIBS) -o $@

fuzzquery.o: fuzzquery.c 
	$(CC) $(FUZZFLAGS) -c -o $@  $^

clean:
	rm -f *.o
	rm -f $(PROG)
	rm -f fuzzquery
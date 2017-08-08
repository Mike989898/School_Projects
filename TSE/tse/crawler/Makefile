# Makefile for 'crawler' module
#
# Michael Kilgore, April 2016

PROG = crawler
OBJS = crawler.o
LIBS = -lcurl
LLIBS = ../lib/cs50ds.a ../common/common.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb
CC = gcc
MAKE = make

# build the crawler
$(PROG): $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

clean:
	rm -f *.o
	rm -f $(PROG)

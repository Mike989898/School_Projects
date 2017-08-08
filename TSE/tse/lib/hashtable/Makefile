# Makefile for 'hashtable' module
#
# Michael Kilgore, April 2016

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb

hasbtable.o:
	$(CC) $(CFLAGS) -c hashtable.c list.c
clean:
	rm -f hashtable.o
	rm -f list.o
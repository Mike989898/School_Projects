# Makefile for 'common' library
#   Builds and compiles all files in the common directory.
#
# Michael Kilgore, April 2016

OBJS = file.o web.o word.o
LIB = common.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb
CC = gcc
MAKE = make

$(LIB): $(OBJS)
	ar cr $@ $^ 

# Rules for .o's
file.o: file.c file.h
web.o: web.c web.h
word.o: word.c word.h
# clean up
clean:
	rm -f *.o
	rm -f $(LIB)
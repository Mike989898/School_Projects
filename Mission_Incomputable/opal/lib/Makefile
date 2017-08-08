# Makefile for 'cs50ds' library
#   Builds and compiles all the data structures into a library.
#
# John Kotz, April 2016

OBJS = memory/memory.o bag/bag.o counters/counters.o hashtable/hashtable.o list/list.o
LIB = cs50ds.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb
CC = gcc
MAKE = make

$(LIB): $(OBJS)
	ar cr $@ $^

# add other targets for the other .o files

list/list.o: list/list.c list/list.h
	cd list && $(MAKE) list.o

hashtable/hashtable.o: hashtable/hashtable.c hashtable/hashtable.h
	cd hashtable && $(MAKE) hashtable.o

bag/bag.o: bag/bag.c bag/bag.h
	cd bag && $(MAKE) bag.o

counters/counters.o: counters/counters.c counters/counters.h
	cd counters && $(MAKE) counters.o

memory/memory.o: memory/memory.c memory/memory.h
	cd memory && $(MAKE) memory.o

fa/fa.o: fa/fa.c fa/fa.h
	cd fa && $(MAKE) fa.o

cd/cd.o: cd/cd.c cd/cd.h
	cd cd && $(MAKE) cd.o
# add cleanup commands for other subdirectories
clean:
	rm -f *~
	rm -f *.o
	rm -f $(LIB)
	cd memory && $(MAKE) $@
	cd bag && $(MAKE) $@
	cd counters && $(MAKE) $@
	cd hashtable && $(MAKE) $@
	cd list && $(MAKE) $@


# Makefile TEMPLATE for 'cs50ds' library
#   Builds and compiles all the data structures into a library.
#
# Michael Kilgore and Akira Murphy, April 2016

OBJS = memory/memory.o bag/bag.o counters/counters.o hashtable/hashtable.o list/list.o indexhelp/indexhelp.o
LIB = cs50ds.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb
CC = gcc
MAKE = make

$(LIB): $(OBJS)
	ar cr $@ $^

# add other targets for the other .o files

bag/bag.o: bag/bag.c bag/bag.h
	cd bag; $(MAKE) bag.o
list/list.o: list/list.c list/list.h
	cd list; $(MAKE) list.o
counters/counters.o: counters/counters.c counters/counters.h
	cd counters; $(MAKE) counters.o
hashtable/hashtable.o: hashtable/hashtable.c hashtable/hashtable.h hashtable/list.c hashtable/list.h
	cd hashtable; $(MAKE) hashtable.o
memory/memory.o: memory/memory.c memory/memory.h
	cd memory; $(MAKE) memory.o
indexhelp/indexhelp.o: indexhelp/indexhelp.c indexhelp/indexhelp.h
	cd indexhelp; $(MAKE) indexhelp.o

# add cleanup commands for other subdirectories
clean:
	rm -f *~
	rm -f *.o
	rm -f $(LIB)
	cd memory; $(MAKE) $@
	cd hashtable; $(MAKE) $@
	cd list; $(MAKE) $@
	cd counters; $(MAKE) $@
	cd bag; $(MAKE) $@
	cd indexhelp; $(MAKE) $@
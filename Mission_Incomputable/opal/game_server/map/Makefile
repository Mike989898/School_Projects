# Makefile for the map
#
# Opal team, May 2016

PROG = map
OBJS = map.o
LIBS = -lm
LLIBS =

GTKFLAGS = `pkg-config --libs gtk+-3.0`
CFLAGS =  -Wall -pedantic -std=c11 -ggdb -lm `pkg-config --cflags gtk+-3.0`
CC = gcc
MAKE = make

# Build the game_server
$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -c  $(GTKFLAGS) 

map.o: map.h

clean:
	rm -f *~
	rm -f *.o
	rm -f $(PROG)
	rm -f *.dSYM



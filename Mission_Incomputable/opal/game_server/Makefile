# Makefile for the gameserver
#
# Opal team, May 2016

PROG = game_server
OBJS = game_server.o map/map.o
LIBS = 
LLIBS = ../commonz/common.a ../lib/cs50ds.a

GTKFLAGS = `pkg-config --libs gtk+-3.0`
CFLAGS = -Wall -pedantic -std=c11 -ggdb -lm `pkg-config --cflags gtk+-3.0`
CC = gcc
MAKE = make

# Build the game_server
$(PROG): $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@ $(GTKFLAGS)
	cd map && make

game_server.o: map/map.h
map.o: map/map.h

clean:
	rm -f *~
	rm -f *.o
	rm -f $(PROG)
	rm -f *.dSYM



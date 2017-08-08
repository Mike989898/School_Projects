OBJS = file.o web.o word.o index.o socket.o pebble_strok.o message.o ../lib/bag/bag.o
LIB = common.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb
CC = gcc
MAKE = make
LIBS = -lm

$(LIB): $(OBJS)
	ar cr $@ $^

file.o: file.c file.h

web.o: web.c web.h

word.o: word.c word.h

index.o: index.c file.o word.o ../lib/cs50ds.a

socket.o: socket.c socket.h

pebble_strok.o: pebble_strok.c pebble_strok.h

message.o: message.c message.h pebble_strok.c pebble_strok.h ../lib/bag/bag.c ../lib/bag/bag.h

../lib/cs50ds.a:
	cd ../lib && $(MAKE)

clean:
	rm -f *~
	rm -f *.o
	rm -f $(LIB)
	rm -f indexTest
	rm -rf *.dSYM
	cd ../lib && $(MAKE) clean;

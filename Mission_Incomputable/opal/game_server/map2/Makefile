CFLAGS=-Wall -pedantic -std=c99 -ggdb
GFLAGS=`pkg-config --cflags --libs gtk+-3.0`
boot:
	gcc $(CFLAGS)  $(GFLAGS) gbootstrap.c -o boot

animate:
	gcc $(CFLAGS)  $(GFLAGS) ganimate.c -o animate
#Makefile template for guide_agent

# Name of the program
PROG = guideagent

#Paths to all dependencies
OBJS = 

#Paths to all archives
LLIBS = ../lib/cs50ds.a ../commonz/common.a

#Compile flags
CFLAGS = -Wall -pedantic -std=c11 -ggdb
CC = gcc
MAKE = make

$(PROG): guideagent.o $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

all: 
	@echo "TODO: make guide_agent..."

clean:
	rm -f *.o
	rm -f *~
	rm -f $(PROG)

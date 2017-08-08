/*
 * socket.h - header file for initializing a UDP DGRAM socket 
 * used for communications between guide agent and game server
 * Chris Bertasi, Michael Kilgore, Andrew Laub
 * May, 2016
 * for use in CS50 final project
 */

#ifndef __SOCKET_H
#define __SOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>      // read, write, close
#include <string.h>      // strlen
#include <strings.h>      // bcopy, bzero
#include <netdb.h>      // socket-related structures
#include <arpa/inet.h>      // socket-related calls
#include <sys/select.h>      // select-related stuff
#include "file.h"      // readline
#include "../lib/list/list.h"
#include "../lib/hashtable/hashtable.h"

//** Type Definitions **//
typedef struct state {
  bool gameRunning;
  bool logRaw;
  FILE* logFile;
  list_t* statsList;
  list_t* fieldAgentList;
  list_t* codeDropList;
  list_t* guideAgentList;
  hashtable_t* fieldAgentTable;
  int gameID;
  long int maxTime;
  long int timeLeft;
  char* guideID;
  char* teamName;
  char* playerName;
  char* gsHost;
  int gsPort;
  struct sockaddr_in* server;
  int socket;
  char* arg1;
  char* arg2;
  void* data;
  long int startTime;
  long int captureTime;
  int numFriends;
  int numRemainingCodeDrops;
  int numActiveFieldAgents;
  char* faList;
  char* cdList;
  bool sentHint;
} state_t;

typedef struct communication {
  int port;
  char* hostName;
  char* msg;
  int msgLen;
} communication_t;



/*
 * Initializes a socket on the passed in hostName and port number
 * for use in the game server
 */
int initializeSocketServer(int port);


/*
 * Initializes a socket on the passed in hostName and port number                                                 * for use in the guide agent
 */
int initializeSocketClient(char* hostName, int port, struct sockaddr_in *other);


/*
 * A loops used for continuous communication over a socket
 * calls a function passed in as a funcion pointer upon receiving stdin
 * calls a function pointer upon receiving from a socket
 */
void checkForInput(state_t* myState, void (*stdinFunc)(), void (*socketFunc)());

/*
 * Sends data to the specified socket
 */
bool sendToOther(int comm_sock, struct sockaddr_in *other, char* msg);


/*
 * Reads and returns data from a socket
 */
communication_t* readFromOther(int comm_sock);

#endif

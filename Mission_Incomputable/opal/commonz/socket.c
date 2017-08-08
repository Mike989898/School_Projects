/*
 * socket.c - file for initializing a UDP DGRAM socket
 * used for communications between guide agent and game server
 * Chris Bertasi, Michael Kilgore, Andrew Laub
 * May, 2016
 * for use in CS50 final project
 */

#include "socket.h"
#include <unistd.h>      // read, write, close
#include <netdb.h>      // socket-related structures
#include <arpa/inet.h>      // socket-related calls
#include <sys/select.h>      // select-related stuff
#include "file.h"      // readline
#include "../lib/memory/memory.h"



int initializeSocketServer(int port)
{
  //initialize socket to listen on
  int comm_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (comm_sock < 0) {
    fprintf(stderr, "Error initializing socket\n");
    exit(6);
  }

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port); //** SPECIFY SPECIFIC PORT HERE **// (0 for random)
  if (bind(comm_sock, (struct sockaddr *) &server, sizeof(server))) {
    fprintf(stderr, "Error binding socket\n");
    exit(6);
  }

  socklen_t serverlen = sizeof(server);
  if (getsockname(comm_sock, (struct sockaddr *) &server, &serverlen)) {
    fprintf(stderr, "Error getting socket name\n");
    exit(6);
  }
  printf("Ready at port %d\n", ntohs(server.sin_port));
  return (comm_sock);
}


int initializeSocketClient(char* hostName, int port, struct sockaddr_in *other)
{

  //find the correct host
  struct hostent *host = gethostbyname(hostName);
  if (host == NULL) {
    fprintf(stderr, "Error: unknown host '%s'\n", hostName);
    
  }

  //Initialize fields of the server address
  other->sin_family = AF_INET;
  bcopy(host->h_addr_list[0], &other->sin_addr, host->h_length);
  other->sin_port = htons(port);

  //Create socket
  int comm_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (comm_sock < 0) {
    fprintf(stderr, "Error opening datagram socket\n");
    //exit(6);
  }

  return comm_sock;
}


void checkForInput(state_t* myState, void (*stdinFunc)(), void (*socketFunc)())
{
  int comm_sock = myState->socket;
  
  fd_set rfds;
  struct timeval timeout;   //how long until timeout
  const struct timeval WAIT = {1,0};//1 second wait

  // Watch stdin (fd 0) and the UDP socket to see when either has input.
  FD_ZERO(&rfds); //first zero everything
  FD_SET(0, &rfds);
  FD_SET(comm_sock, &rfds);
  int nfds = comm_sock+1;//highest-numbered fd in rfds + 1


  timeout = WAIT;
  int response = select(nfds, &rfds, NULL, NULL, &timeout);
  do {
    timeout = WAIT;
    if (response < 0) { //something went wrong
      fprintf(stderr, "Error with select()\n");
      exit(4);
    } else if (response == 0) {
      //timeoout
    } else if (response > 0) {
      //input at either stdin or the socket
      if (FD_ISSET(0, &rfds)) { //stdin has input
	char* line = readline(stdin);
	if (line != NULL) {
	  stdinFunc(line, myState);//call the function pointer to handle stdin
	  free(line);
	}
      }
      if (FD_ISSET(comm_sock, &rfds)) { //socket has input
	communication_t* cmn = readFromOther(comm_sock);
	socketFunc(cmn, myState);
	//cleanup and free allocated memory
	free(cmn->msg);
	free(cmn->hostName);
	free(cmn);
      }
    }
    fflush(stdout);
    response = select(nfds, &rfds, NULL, NULL, &timeout);
  } while (response > 0);

}

bool sendToOther(int comm_sock, struct sockaddr_in* other, char* msg)
{
  if (other->sin_family != AF_INET) {
    fprintf(stderr, "Error: target is not a valid destination\n");
    return false;
  }
  
  if (sendto(comm_sock, msg, strlen(msg), 0, (struct sockaddr *) other, sizeof(*other)) < 0){
    fprintf(stderr, "Error sending UDP datagram!\n");
    return false;
  }

  return true;
}

communication_t* readFromOther(int comm_sock)
{
  // socket has input ready
  struct sockaddr_in sender; // sender of this message
  struct sockaddr *senderp = (struct sockaddr *) &sender;
  socklen_t senderlen = sizeof(sender);  // must pass address to length
  const int BUFSIZE = 8192;
  char buf[BUFSIZE];      // buffer for reading data from socket
  int msgLen = recvfrom(comm_sock, buf, BUFSIZE-1, 0, senderp, &senderlen);

  if (msgLen < 0) {
    fprintf(stderr, "Error receiving from socket\n");
    exit(1);
  } else {
    buf[msgLen] = '\0';//add a null terminating char
    if (sender.sin_family != AF_INET)
      printf("Unexpected source of data: sender isn't AF_INET\n");
    else {
    }
    fflush(stdout); //make sure all data is printed out
  }
  communication_t* cmn = count_malloc_assert(sizeof(communication_t), "communication");
  cmn->msg = count_malloc_assert(8192, "msg");
  cmn->port = ntohs(sender.sin_port);
  cmn->hostName = count_malloc_assert(sizeof(char) * 100, "hostName");
  cmn->msgLen = msgLen;
  strcpy(cmn->msg, buf);
  strcpy(cmn->hostName, inet_ntoa(sender.sin_addr));
  return cmn;
}

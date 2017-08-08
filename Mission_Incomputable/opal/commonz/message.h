/*
 * message.h - holds data structs relevent to cs50 final project
 * as well as prototype for parsing function
 * Chris Bertasi, Michael Kilgore, Andrew Laub
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../lib/list/list.h"
#include "pebble_strok.h"

#ifndef __MESSAGE_H
#define __MESSAGE_H

/*Field agent struct*/
typedef struct fa {
  char* pebbleId;
  float lat;
  float lng;
  char* playerName;
  bool playerStatus;
  char* teamName;
  long int lastContact;
} fa_t;

/*Guide agent struct*/
typedef struct ga {
  int gameID;
  char* guideID;
  char* teamName;
  char* playerName;
  long int lastContact;
} ga_t;

/*Code drop struct*/
typedef struct cd {
  float lat;
  float lng;
  char* hexCode;
  char* neutralizingTeam;
} cd_t;

typedef struct team {
char *teamName; 
int numPlayers;
int numCaptures;
int numCaptured;
int numNeutralized;
} team_t;

/*message struct*/
typedef struct msg {
  char *opCode;
  int gameId;
  long int timeLeft;
  char* guideId;
  char* pebbleId;
  float lat;
  float lng;
  char *codeId;
  char *teamName;
  char *playerName;
  int statusReq;
  char* message;
  int numRemainingCodeDrops;
  int numFriendlyOperatives;
  int numFoeOperatives;
  char* captureId;
  char* respCode;
  list_t *faList;
  list_t *cdList;
  list_t *team_record_list;
} msg_t;

/*
 * Message parsing function that will parse tha passed in message and return a pointer
 * to a message struct
 */
msg_t *message_parse(char *message_string);


/*
 * Free's all malloc'ed memory
 */
void message_delete(msg_t* msg);

#endif // __MESSAGE_H

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
#include "pebble_strok.h"

#ifndef __MESSAGE_H
#define __MESSAGE_H

/*message struct*/
typedef struct msg {
  char *opCode;
  int gameId;
  int guideId;
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
  long int timeLeft;
  char *gameInfo;
} msg_t;

/*
 * Message parsing function that will parse tha passed in message and return a pointer
 * to a message struct
 */
msg_t *message_parse(char *message_string);

#endif // __MESSAGE_H

/*
 * message.c - file for parsing messages
 * Chris Bertasi, Michael Kilgore, Andrew Laub
 * May, 2016
 * for use in CS50 final project
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "message.h"
#include "pebble_strok.h"
#include <pebble.h>


/*Internal Prototypes*/
static msg_t *message_new();
static void ga_hint_parse(msg_t *message);
static void gs_capture_id_parse(msg_t *message);
static void gs_response_parse(msg_t *message);
static void game_status_parse(msg_t *message, char* str);
static void field_agent_parse(msg_t *message);
/*static void game_over_parse(msg_t *message);*/

/*message_parse()*/
msg_t *message_parse(char *message_string)
{
  msg_t *message = message_new();
  char* copy = malloc(strlen(message_string) + 1);
  strcpy(copy, message_string);
  char *op_code = pebble_strtok(message_string, "|");
  if(op_code == NULL){
    return NULL; //couldn't split of the op code
  }
  //copy the strtok result
  message->opCode = malloc(strlen(op_code) + 1);
  strcpy(message->opCode, op_code);

  if ((strcmp(op_code, "GA_HINT")) == 0){
    ga_hint_parse(message);
  }
  else if ((strcmp(op_code, "GS_CAPTURE_ID")) == 0){
    gs_capture_id_parse(message);
  }
  else if ((strcmp(op_code, "GAME_STATUS")) == 0){
    game_status_parse(message, copy);
  }
  else if ((strcmp(op_code, "GS_RESPONSE")) == 0){
    gs_response_parse(message);
  }
/*  else if ((strcmp(op_code, "GAME_OVER")) == 0){
    game_over_parse(message);
  }*/
  free(copy);
  return message;
}

/*
 * this one is tricky, as it could be sent to field agents or guide agents
 * this method first checks which other helper methods to call
 */
static void game_status_parse(msg_t *message, char* str)
{
    field_agent_parse(message);
}


/*
 * 4 tokens long including op code
 */
static void gs_response_parse(msg_t * message)
{
  char* field;
  for (int i = 0; i < 3; i++) {
    field = pebble_strtok(NULL, "|");
    if (field == NULL) {
      return; //message was formatted wrong
    }
    char* temp = malloc(strlen(field) + 1);
    strcpy(temp, field);
    switch (i) {
    case 0:
      message->gameId = atoi(temp);
      free(temp);
      break;
    case 1:
      message->respCode = temp;
      break;
    case 2:
      message->message = temp;
      break;
    }
  }
}


/*
 * 3 tokens long, including op code
 */
static void gs_capture_id_parse(msg_t * message)
{
  char* field;
  for (int i = 0; i < 2; i++) {
    field = pebble_strtok(NULL, "|");
    if (field == NULL) {
      return; //message was formatted wrong
    }
    char* temp = malloc(strlen(field) + 1);
    strcpy(temp, field);
    switch (i) {
    case 0:
      message->gameId = atoi(temp);
      free(temp);
      break;
    case 1:
      message->captureId = temp;
      break;
    }
  }
}

/*
 * 7 tokens long, including the op code
 */
static void ga_hint_parse(msg_t *message)
{
  char* field;
  for (int i = 0; i < 6; i++) {
    field = pebble_strtok(NULL, "|");
    if (field == NULL) {
      return; //message was formatted wrong
    }
    char* temp = malloc(strlen(field) + 1);
    strcpy(temp, field);
    switch (i) {
    case 0:
      message->gameId = atoi(temp);
      free(temp);
      break;
    case 1:
      message->guideId = atoi(temp);
      free(temp);
      break;
    case 2:
      message->teamName = temp;
    break;
    case 3:
      message->playerName = temp;
      break;
    case 4:
      message->pebbleId = temp;
      break;
    case 5:
      message->message = temp;
      break;
    }
  }
}

/*
 * Called when sending a game_status opCode to a field agent
 */ 
static void field_agent_parse(msg_t* message)
{
  char* field;
  for (int i = 0; i < 6; i++) {
    field = pebble_strtok(NULL, "|");
    if (field == NULL) {
      return; //message was formatted wrong
    }
    char* temp = malloc(strlen(field) + 1);
    strcpy(temp, field);
    switch (i) {
    case 0:
      message->gameId = atoi(temp);
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Game id parsed value: %d",message->gameId);
      free(temp);
      break;
    case 1:
      message->guideId = atoi(temp);
      free(temp);
      break;
    case 2:
      message->numRemainingCodeDrops = atoi(temp);
      free(temp);
      break;
    case 3:
      message->numFriendlyOperatives = atoi(temp);
      free(temp);
      break;
    case 4:
      message->numFoeOperatives = atoi(temp);
      free(temp);
      break;
    case 5:
      message->timeLeft = atol(temp);
      free(temp);
      break;
    }
  }
}

/*static void game_over_parse(msg_t *message){
  char* field;
  for (int i = 0; i < 3; i++) {
    field = pebble_strtok(NULL, "|");
    if (field == NULL) {
      return; //message was formatted wrong
    }
    char* temp = malloc(strlen(field) + 1);
    strcpy(temp, field);
  switch (i) {
    case 0:
      message->gameId = atoi(temp);
      free(temp);
      break;
    case 1:
      message->numRemainingCodeDrops = atoi(temp);
      free(temp);
      break;
    case 2:
      message->gameInfo = temp;
      free(temp);
      break;
  }
}*/


/*message_new()*/
static msg_t *message_new(){
  //create new message and malloc space for it
  msg_t *new_message = malloc(sizeof(msg_t));
  if(new_message == NULL){
    return NULL;
  }
  //initialize msg_t struct fields
  new_message->opCode = NULL;
  new_message->gameId = 0;
  new_message->guideId = 0;
  new_message->lat = 0.0;
  new_message->lng = 0.0;
  new_message->codeId = NULL;
  new_message->teamName = NULL;
  new_message->playerName = NULL;
  new_message->statusReq = 0;
  new_message->pebbleId = NULL;
  new_message->message = NULL;
  new_message->numRemainingCodeDrops = 0;
  new_message->numFriendlyOperatives = 0;
  new_message->numFoeOperatives = 0;
  new_message->captureId = NULL;
  new_message->respCode = NULL;
  new_message->timeLeft = 0;
  return new_message;
}


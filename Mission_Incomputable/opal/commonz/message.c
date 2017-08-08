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
#include "../lib/list/list.h"
#include "message.h"
#include "pebble_strok.h"
#include "../lib/bag/bag.h"


/*Internal Prototypes*/
static msg_t *message_new();
static void fa_location_parse(msg_t *message);
static void fa_neutralize_parse(msg_t *message);
static void fa_capture_parse(msg_t *message);
static void ga_status_parse(msg_t *message);
static void ga_hint_parse(msg_t *message);
static void gs_capture_id_parse(msg_t * message);
static void gs_response_parse(msg_t * message);
static void game_status_parse(msg_t *message, char* str);
static void game_over_parse(msg_t *message);
//static void message_print(msg_t *message); //testing use only
static void field_agent_parse(msg_t* message);
static void guide_agent_parse(msg_t* message, char* stuff);

/*
 * testing use only
 */

/*
int main()
{
  char text[] = "GAME_STATUS|1234|1234,Opal,me,true,123.213,312.4324,1000|4400|1100,123.23121,1232.3213,NONE";
  msg_t* msg = message_parse(text);
  message_print(msg);
}
*/


void message_delete(msg_t* msg)
{
  if (strcmp(msg->opCode, "FA_LOCATION") == 0) {
    free(msg->pebbleId);
    free(msg->teamName);
    free(msg->playerName);
  }
  else if (strcmp(msg->opCode, "FA_NEUTRALIZE") == 0) {
    free(msg->pebbleId);
    free(msg->teamName);
    free(msg->playerName);
    free(msg->codeId);
  }
  else if (strcmp(msg->opCode, "FA_CAPTURE") == 0) {
    free(msg->pebbleId);
    free(msg->teamName);
    free(msg->playerName);
    free(msg->captureId);
  }
  else if (strcmp(msg->opCode, "GA_STATUS") == 0) {
    free(msg->guideId);
    free(msg->teamName);
    free(msg->playerName);
  }
  else if (strcmp(msg->opCode, "GA_HINT") == 0) {
    free(msg->guideId);
    free(msg->teamName);
    free(msg->playerName);
    free(msg->message);
    free(msg->pebbleId);
  }
  else if (strcmp(msg->opCode, "GAME_STATUS") == 0) {
    if (msg->guideId != NULL) { //to field agents
      free(msg->guideId);
    }
  }
  else if (strcmp(msg->opCode, "GS_CAPTURE_ID") == 0) {
    free(msg->captureId);
  }
  else if (strcmp(msg->opCode, "GS_RESPONSE") == 0) {
    free(msg->respCode);
    free(msg->message);
  }
  
  free(msg->opCode);
  free(msg);
}



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

  if ((strcmp(op_code, "FA_LOCATION")) == 0){
    fa_location_parse(message);
  }
  else if ((strcmp(op_code, "FA_NEUTRALIZE")) == 0){
    fa_neutralize_parse(message);
  }
  else if ((strcmp(op_code, "FA_CAPTURE")) == 0){
    fa_capture_parse(message);
  }
  else if ((strcmp(op_code, "GA_STATUS")) == 0){
    ga_status_parse(message);
  }
  else if ((strcmp(op_code, "GA_HINT")) == 0){
    ga_hint_parse(message);
  }
  else if ((strcmp(op_code, "GS_CAPTURE_ID")) == 0){
    gs_capture_id_parse(message);
  }
  else if ((strcmp(op_code, "GAME_STATUS")) == 0){
    game_status_parse(message, copy);
  }
  else if ((strcmp(op_code, "GAME_OVER")) == 0){
    game_over_parse(message);
  }
  else if ((strcmp(op_code, "GS_RESPONSE")) == 0){
    gs_response_parse(message);
  }
  free(copy);
  return message;
}

/*
 * 4 tokens including op code
 */
static void game_over_parse(msg_t *message)
{
  char* field;
  int count = 0;
  for (int i = 0; i < 3; i++) {
    field = pebble_strtok(NULL, "|");
    if (field == NULL) {
      return; //message was formatted wrong
    }
    if (i == 2 && strcmp(field, " ") == 0) { //" " represents empty record list
      message->team_record_list = list_new(); //initialize the list of teams
      return; //no records to parse
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
    case 2: //the hard part - the list of entries
      for (int i = 0; temp[i] != '\0'; i++) {
	if (temp[i] == ':')
	  count++;
      }//count now holds the number of delimeters
      message->team_record_list = list_new(); //initialize the list of teams
      bag_t* tokens = bag_new(); //holds the team records
      char* val;
      val = pebble_strtok(temp, ":");//val now has first record
      
      char* str = malloc(strlen(val) + 1); 
      strcpy(str, val); //str is now an editable copy of the first record
      bag_insert(tokens, str); //insert copy to the bag
      
      for (int i = 0; i < count; i++) {
	val = pebble_strtok(NULL, ":");
	str = malloc(strlen(val) + 1);
	strcpy(str, val);
	bag_insert(tokens, str);
      } //bag now holds all the team records
      
      
      for (int i = 0; i < count + 1; i++) {
	char* record = bag_extract(tokens); //grab a record, the parse it
	team_t* team = malloc(sizeof(team_t));//create this team
	
	char* data = pebble_strtok(record, ",");
	char* copy = malloc(strlen(data) + 1);
	strcpy(copy, data); //spot now holds a copy of teamName
	
	team->teamName = copy;

	data = pebble_strtok(NULL, ","); //data now holds numPlayers
	team->numPlayers = atoi(data);

	data = pebble_strtok(NULL, ","); //data now holds numCaptures
	team->numCaptures = atoi(data);

	data = pebble_strtok(NULL, ","); //data now holds numCaptured
	team->numCaptured = atoi(data);

	data = pebble_strtok(NULL, ","); //data now holds numNeutralized
	team->numNeutralized = atoi(data);

	list_insert(message->team_record_list, team->teamName, team); //list is keyed by teamName
      }
      
      bag_delete(tokens, free);
      free(temp);
      break;
    }
  }
}

/*
 * this one is tricky, as it could be sent to field agents or guide agents
 * this method first checks which other helper methods to call
 */
static void game_status_parse(msg_t *message, char* str)
{
  int count = 0;
  for (int i = 0; str[i] != '\0'; i++) {
    if (str[i] == '|')
      count++;
  }//count now holds the number of delimeters
  if (count == 4) { //headed to guide agent.
    guide_agent_parse(message, str);
  } else if (count == 6) { //headed to field agent
    field_agent_parse(message);
  } else {
    //improperly formatted
  }
}

/*
 * Called when sending a game_status opCode to a guide agent
 */
static void guide_agent_parse(msg_t* message, char* stuff)
{
  char* field;
  int count = 0;
  bag_t* tokens;
  char* val;
  char* str;
  char* dup = malloc(strlen(stuff) + 1);
  strcpy(dup,stuff);
  for (int i = 0; i < 4; i++) {
    field = pebble_strtok(NULL, "|");
    if (field == NULL) {
      if (i == 2) {
	field = pebble_strtok(stuff, "|");
	field = pebble_strtok(NULL, "|");
	field = pebble_strtok(NULL, "|");
	field = pebble_strtok(NULL, "|");
      }
      else if (i == 3) {
	field = pebble_strtok(dup, "|");
	field = pebble_strtok(NULL, "|");
	field = pebble_strtok(NULL, "|");
	field = pebble_strtok(NULL, "|");
	field = pebble_strtok(NULL, "|");
      }
      else {
	return; //message was formatted wrong
      }
    }
    char* temp = malloc(strlen(field) + 1);
    strcpy(temp, field);
    switch (i) {
    case 0:
      message->gameId = atoi(temp);
      free(temp);
      break;
    case 1: //begin parsing the list of field agents
      for (int i = 0; temp[i] != '\0'; i++) {
	if (temp[i] == ':')
	  count++;
      }//count now holds the number of delimeters 
      message->faList = list_new(); //initialize the list
      if (strcmp(temp, " ") == 0) { //empty field, no work to do for parsing
	break;
      }
      tokens = bag_new(); //holds the list of field agent records
      val = pebble_strtok(temp, ":");//val now has first record
      
      str = malloc(strlen(val) + 1);
      strcpy(str, val); //str is now an editable copy of the first record
      bag_insert(tokens, str); //insert copy to the bag
      
      for (int i = 0; i < count; i++) {
	val = pebble_strtok(NULL, ":");
	str = malloc(strlen(val) + 1);
	strcpy(str, val);
	bag_insert(tokens, str);
      } //bag now holds all the field agent records 
      
      for (int i = 0; i < count + 1; i++) {
	char* record = bag_extract(tokens); //grab a record, the parse it
	fa_t* fieldAgent = malloc(sizeof(fa_t));//create this fieldAgent

	char* data = pebble_strtok(record, ","); //data holds pebbleId
	char* copy = malloc(strlen(data) +1);
	strcpy(copy, data);  //copy is now a copy of pebbleId
	fieldAgent->pebbleId = copy;
	
	data = pebble_strtok(NULL, ","); //data now holds teamName
	copy = malloc(strlen(data) +1);
	strcpy(copy, data); //copy now holds a copy of teamName
	fieldAgent->teamName = copy;

	data = pebble_strtok(NULL, ","); //data now holds playerName
	copy = malloc(strlen(data) + 1);
	strcpy(copy, data);
	fieldAgent->playerName = copy;

	data = pebble_strtok(NULL, ","); //data now holds playerStatus
	if (strcmp(data, "true") == 0) {
	  fieldAgent->playerStatus = true;
	} else {
	  fieldAgent->playerStatus = false;
	}
	
	data = pebble_strtok(NULL, ","); //data now holds lastKnownLat
	fieldAgent->lat = (float) atof(data);

	data = pebble_strtok(NULL, ","); //data now holds lastKnownLong
	fieldAgent->lng = (float) atof(data);

	data = pebble_strtok(NULL, ","); //data now holds secondsSinceLastContact
	fieldAgent->lastContact = atoi(data);
	//insert this field agent to the list
	
	list_insert(message->faList, fieldAgent->pebbleId, fieldAgent); //list is keyed by pebbleId (hex)

	free(record);
      }
      bag_delete(tokens, free);
      free(temp);
      break;
    case 2: //begin parsing the list of code drops
      count = 0;
      for (int i = 0; temp[i] != '\0'; i++) {
	if (temp[i] == ':')
	  count++;
      }//count now holds the number of delimeters
      message->cdList = list_new(); //initialize the list

      tokens = bag_new(); //holds the list of code drop records
      val = pebble_strtok(temp, ":");//val now has first record

      str = malloc(strlen(val) + 1);
      strcpy(str, val); //str is now an editable copy of the first record
      bag_insert(tokens, str); //insert copy to the bag

      for (int i = 0; i < count; i++) {
	val = pebble_strtok(NULL, ":");
	str = malloc(strlen(val) + 1);
	strcpy(str, val);
	bag_insert(tokens, str);
      } //bag now holds all the code drop records 

      for (int i = 0; i < count + 1; i++) {
	char* record = bag_extract(tokens); //grab a record, the parse it
	cd_t* codeDrop = malloc(sizeof(cd_t));//create this code drop
	
	char* data = pebble_strtok(record, ","); //data holds codeId
	char* copy = malloc(strlen(data) + 1);
	strcpy(copy, data);
	codeDrop->hexCode = copy;
		
	data = pebble_strtok(NULL, ","); //data now holds lat
	codeDrop->lat = (float) atof(data);
	
	data = pebble_strtok(NULL, ","); //data now holds long
	codeDrop->lng = (float) atof(data);
	
	data = pebble_strtok(NULL, ","); //data now holds neutralizingTeam
	copy = malloc(strlen(data) + 1);
	strcpy(copy, data);
	codeDrop->neutralizingTeam = copy;
	
	//insert this field agent to the list
	list_insert(message->cdList, codeDrop->hexCode, codeDrop); //list is keyed by codeId(hexcode)
	free(record);
      }
      bag_delete(tokens, NULL);
      break;
    case 3:
      message->timeLeft = atol(temp);
      free(temp);
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
  for (int i = 0; i < 5; i++) {
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
      message->guideId = temp;
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
    }
  }
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
 * 6 tokens long, including the op code
 */
static void fa_capture_parse(msg_t *message)
{
  char* field;
  for (int i = 0; i < 5; i++) {
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
      message->pebbleId = temp;
      break;
    case 2:
      message->teamName = temp;
      break;
    case 3:
      message->playerName = temp;
    case 4:
      message->captureId = temp;
      break;
    }
  }
}


/*
 * 8 long, including the op code
 */ 
static void fa_neutralize_parse(msg_t *message)
{
  char* field;
  for (int i = 0; i < 7; i++) {
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
      message->pebbleId = temp;
      break;
    case 2:
      message->teamName = temp;
      break;
    case 3:
      message->playerName = temp;
    case 4:
      message->lat = (float) atof(temp);
      free(temp);
      break;
    case 5:
      message->lng = (float) atof(temp);
      free(temp);
      break;
    case 6:
      message->codeId = temp;
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
      message->guideId = temp;
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
 * Should have 6 fields including the op code
 * 
 */
static void ga_status_parse(msg_t *message)
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
      message->guideId = temp;
      break;
    case 2:
      message->teamName = temp;
      break;
    case 3:
      message->playerName = temp;
      break;
    case 4:
      message->statusReq = atoi(temp);
      free(temp);
      break;
    }
  }
  return;
}


/*fa_location_parse()*/
/*
 *This message should have 8 fields including the opcode
 *If there aren't 8 fields or one more more of the fields are
 *invalid, return NULL
 */
static void fa_location_parse(msg_t *message){
  int i;
  for(i=0; i<7; i++){
    char *field = pebble_strtok(NULL, "|");
    if(field == NULL){
      return; //couldn't split off a field
    }
    char *temp_field = malloc(strlen(field)+ 1);
    strcpy(temp_field, field);
    switch(i){
    case 0:
      message->gameId = atoi(temp_field);
      free(temp_field);
      break;
    case 1:
      message->pebbleId = temp_field;
      break;
    case 2:
      message->teamName = temp_field;
      break;
    case 3:
      message->playerName = temp_field;
      break;
    case 4:
      message->lat = atof(temp_field);
      free(temp_field);
      break;
    case 5:
      message->lng = atof(temp_field);
      free(temp_field);
      break;
    case 6:
      message->statusReq = atoi(temp_field);
      free(temp_field);
      break;
    }
  }
  return;
}



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
  new_message->guideId = NULL;
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
  new_message->faList = NULL;
  new_message->cdList = NULL;
  new_message->team_record_list = NULL;
  new_message->timeLeft = 0;
  return new_message;
}

/*message_print()*/
/*
 * For use in debugging
 */

/*
static void message_print(msg_t *message){
  if(message->opCode == NULL){
    printf("opcode: %s\n", "null");
  }
  else{
    printf("opcode: %s\n", message->opCode);
  }
  printf("Game ID: %d\n", message->gameId);
  printf("Guide ID: %s\n", message->guideId);
  printf("Pebble ID: %s\n", message->pebbleId);
  printf("lat: %f\n", message->lat);
  printf("lat: %f\n", message->lng);
  if(message->codeId == NULL){
    printf("codeId: %s\n", "null");
  }
  else{
    printf("codeId: %s\n", message->codeId);
  }
  if(message->teamName == NULL){
    printf("teamName: %s\n", "null");
  }
  else{
    printf("teamName: %s\n", message->teamName);
  }
  printf("statusRequest: %d\n", message->statusReq);
  if(message->message == NULL){
    printf("message: %s\n", "null");
  }
  else{
    printf("message: %s\n", message->message);
  }
  printf("numRemainingCodeDrops: %d\n", message->numRemainingCodeDrops);
  printf("numFriendlyOperatives: %d\n", message->numFriendlyOperatives);
  printf("numFoeOperatives: %d\n", message->numFoeOperatives);
  if (message->captureId == NULL) {
    printf("captureId: %s\n", "null");
  }
  else {
    printf("captureId: %s\n", message->captureId);
  }
  if (message->respCode == NULL) {
    printf("respCode: %s\n", "null");
  }
  else {
    printf("respCode: %s\n", message->respCode);
  }
  if (message->team_record_list == NULL) {
    printf("team_record_list: %s\n", "null");
  }
  else {
    printf("team_record_list:\n");
    for (int i = 1; i < 4; i++) {
      char name[10];
      sprintf(name, "team%d", i);
      team_t* team = list_find(message->team_record_list, name);
      printf("team name: %s\n", team->teamName);
      printf("numPlayers: %d\n", team->numPlayers);
      printf("numCaptures: %d\n", team->numCaptures);
      printf("numCaptured: %d\n", team->numCaptured);
      printf("numNeutralized: %d\n\n", team->numNeutralized);
    }
  }
  if (message->faList == NULL) {
    printf("faList: %s\n", "null");
  }
  else {
    printf("faList:\n");
    for (int i = 1; i < 4; i++) {
      char name[5];
      sprintf(name, "%d", i);
      fa_t* fa = list_find(message->faList, name);
      printf("pebbleID: %s\n", fa->pebbleId);
      printf("teamName: %s\n", fa->teamName);
      printf("playerName: %s\n", fa->playerName);
      printf("playerStatus: %s\n",fa->playerStatus ? "true":"false" );
      printf("lastKnownLat: %f\n", fa->lat);
      printf("lastKnownLong: %f\n", fa->lng);
      printf("secondsSinceLastContact: %d\n", fa->lastContact);
    }
  }
  if (message->cdList == NULL) {
    printf("cdList: %s\n", "null");
  }
  else {
    printf("cdList:\n");
    for (int i = 1; i < 3; i++) {
      char name[7];
      sprintf(name, "%d%d%d%d", i,i,i,i);
      cd_t* cd = list_find(message->cdList, name);
      printf("codeID: %s\n", cd->hexCode);
      printf("lat: %f\n", cd->lat);
      printf("long: %f\n", cd->lng);
      printf("neutralizingTeam: %s\n", cd->neutralizingTeam);
    }
  }
}
*/

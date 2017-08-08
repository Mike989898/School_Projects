/*
 * Game server for cs50 final project
 * Chris Bertasi, Michael Kilgore, Andrew Laub
 * May 2016
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include "../lib/list/list.h"
#include "../lib/hashtable/hashtable.h"
#include "../lib/memory/memory.h"
#include "../commonz/message.h"
#include "../commonz/socket.h"
#include "../commonz/file.h"
#include "../commonz/pebble_strok.h"
#include "map/map.h"


// --------- Types ------------
typedef struct faInfo {
  fa_t* fa;
  communication_t* cmn;
  char* captureCode;
  struct sockaddr_in* agent;
  int socket;
  int numCaptures;
  int numNeutralized;
} faInfo_t;

typedef struct gaInfo {
  ga_t* ga;
  communication_t* cmn;
  char* captureCode;
  struct sockaddr_in* agent;
  int socket;
} gaInfo_t;

typedef struct teamStats {
  char* teamName;
  int numPlayers;
  int numCaptures;
  int numCaptured;
  int numNeutralized;
} teamStats_t;


// ------ Function Prototypes -
char* currTime(void);
list_t* loadDrops(FILE* dropFile, state_t* myState);
void handleStdIn(char* inp, state_t* myState);
void handleMsg(communication_t* cmn, state_t* myState);
void faLoc(msg_t* msg, communication_t* cmn, state_t* myState);
void faNeutralize(msg_t* msg, communication_t* cmn, state_t* myState);
void faCapture(msg_t* msg, communication_t* cmn, state_t* myState);
void faCaptureHelper(void* myStateVoid, char* key, void* faInfoVoid);
void gaStatus(msg_t* msg, communication_t* cmn, state_t* myState);
void gaHint(msg_t* msg, communication_t* cmn, state_t* myState);
void gsResponse(msg_t* msg, communication_t* cmn, state_t* myState, char* respCode, char* text);
bool withinTen(float lat1, float lng1, float lat2, float lng2);
void generateCaptureCodes(void* msgVoid, char* id, void* faInfoVoid);
void sendCaptureMessage(faInfo_t* faInfo, state_t* myState);
void faStatusReq(msg_t* msg, faInfo_t* faInfo, state_t* myState);
void gaStatusReq(msg_t* msg, gaInfo_t* gaInfo, state_t* myState);
void numFriendlyAgents(void* myStateVoid, char* key, void* faInfoVoid);
void codeDropStringHelper(void* myStateVoid, char* key, void* cdVoid);
void fieldAgentStringHelper(void* myStateVoid, char* key, void* faInfoVoid);
void gameOver(state_t* myState);
void gameOverMsgField(void* statsListVoid, char* key, void* faInfoVoid);
void gameOverMsgGuide(void* statsListVoid, char* key, void* gaInfoVoid);
void generateStatsString(void* myStateVoid, char* key, void* teamStatsVoid);
void gameOverMsgGuideSend(void* myStateVoid, char* key, void* gaInfoVoid);
void gameOverMsgFieldSend(void* myStateVoid, char* key, void* faInfoVoid);


// ---------Functions----------

// --- Variables needed by helpers

// ---------- Main ------------

int main(int argc, char* argv[]) {

  //seed the random number generator
  srand(time(NULL)); //use the current time for a good seed
  state_t* myState = count_malloc_assert(sizeof(state_t), "myState");
  myState->maxTime = 10;
  myState->gameID = rand() % 65535; //picks a number from [0,65535]

  char* filePath;
  FILE* dropFile;

  //Ensure log dir exists
  system("mkdir ../log");
  //Open the logfile in logdir
  myState->logFile = fopen("../log/gameserver.log", "a");

  //Set capture time to arbitrarily small time
  myState->captureTime = 0;

  myState->numActiveFieldAgents = 0;

  //Parsing arguments
  if (argc != 3 && argc != 4) {
    fprintf(stderr, "Takes two to three arguments, not %d\nUsage: ./game_server [-v] codeDropPath GSport\n", argc);
    exit(1);
  }

  if (argc == 3) {
    myState->gsPort = strtol(argv[1], NULL, 10);
    filePath = count_malloc(strlen(argv[2]) + 1);
    strcpy(filePath, argv[2]);

    if ((dropFile = fopen(filePath, "r")) == NULL) {
      fprintf(stderr, "Could not open file at location %s\n", filePath);
      count_free(filePath);
      exit(2);
    }

    count_free(filePath);
    myState->logRaw = false;
  }

  else {
    if (strcmp(argv[1], "-v") != 0){
      fprintf(stderr, "Invalid first argument. Usage: ./game_server [-v] codeDropPath GSport\n");
      exit(3);
    }

    myState->gsPort = strtol(argv[2], NULL, 10);
    filePath = count_malloc(strlen(argv[2]) + 1);
    strcpy(filePath, argv[3]);

    if ((dropFile = fopen(filePath, "r")) == NULL) {
      fprintf(stderr, "Could not open file at location %s\n", filePath);
      count_free(filePath);
      exit(2);
    }

    count_free(filePath);
    myState->logRaw = true;
  }

  //Args ok; start official game time
  myState->startTime = time(NULL);

  myState->gameRunning = true;

  //log info

  fprintf(myState->logFile, "[%s] Game started at: Port number is %d\n", currTime(), myState->gsPort);

  myState->numRemainingCodeDrops = 0;
  myState->codeDropList = loadDrops(dropFile, myState);
  fprintf(myState->logFile, "[%s] Code drops loaded\n", currTime());

  myState->fieldAgentTable = hashtable_new(100);
  myState->guideAgentList = list_new();

  struct sockaddr_in them = {0, 0, {0}};  // sender's addres
  myState->server = &them;
  myState->socket = initializeSocketServer(myState->gsPort);

  while(myState->gameRunning) {
    checkForInput(myState, handleStdIn, handleMsg);
    if (myState->numRemainingCodeDrops == 0)
      myState->gameRunning = false;
    if (myState->maxTime < time(NULL) - myState->startTime) {
      fprintf(myState->logFile, "Game has been running for longer than &ld seconds. Ending game...");
      myState->gameRunning = false;
    }
  }

  gameOver(myState);


  //clean up
  fprintf(myState->logFile, "\nLogging off...\n");
  fflush(myState->logFile);
  fclose(myState->logFile);
  close(myState->socket);
  free(myState);
  count_report(stdout, "server memory stats");

}

// ------- Functions ----------

char* currTime(void) {
  time_t now = time(NULL);
  struct tm* localTime = localtime(&now);
  char* time = asctime(localTime);
  strtok(time, "\n");
  return time;
}

list_t* loadDrops(FILE* dropFile, state_t* myState) {
  list_t* drops = list_new();
  for (char* line = readline(dropFile); line != NULL; line = readline(dropFile)) {
    cd_t* drop = count_malloc(sizeof(cd_t));
    drop->lat = atof(strtok(line, " "));
    drop->lng = atof(strtok(NULL, " "));
    char* code = strtok(NULL, " ");
    drop->hexCode = code;
    drop->neutralizingTeam = "none\0";
    printf("%s\n", drop->hexCode);
    list_insert(drops, code, drop);
    myState->numRemainingCodeDrops++;
  }
  return drops;
}

void handleStdIn(char* inp, state_t* myState) {
  fprintf(myState->logFile, "[%s] User stdin: %s\n", currTime(), inp);
  if (strcmp(inp, "map") != 0) {
    if (strcmp(inp, "endGame") != 0)
      printf("The only stdinputs recognized by the game server are `map` and `endGame'\n");
    else myState->gameRunning = false;
  }
  else {
    fprintf(myState->logFile, "Opening the GTK map...\n");
    openMap();
  }
}

void handleMsg(communication_t* cmn, state_t* myState) {
  if (cmn == NULL) {
    fprintf(myState->logFile, "No communication\n");
    return;
  }
  if (cmn->msg == NULL) {
    fprintf(myState->logFile, "No message in communication\n");
    return;
  }
  if (myState->logRaw) { //log all messages received
    fprintf(myState->logFile, "[%s] Received message: %s\n", currTime(), cmn->msg);
  }
  msg_t* msg = message_parse(cmn->msg);

  if (msg == NULL) {
    fprintf(myState->logFile, "Parsing error, null message\n");
    return;
  }
  if (msg->opCode == NULL) {
    fprintf(myState->logFile, "No opcode in message\n");
    return;
  }

  char* code = msg->opCode;

  if (strcmp(code, "FA_LOCATION")==0){
    faLoc(msg, cmn, myState);
  }
  else if (strcmp(code, "FA_NEUTRALIZE")==0)
    faNeutralize(msg, cmn, myState);
  else if (strcmp(code, "FA_CAPTURE")==0)
    faCapture(msg, cmn, myState);
  else if (strcmp(code, "GA_STATUS")==0)
    gaStatus(msg, cmn, myState);
  else if (strcmp(code, "GA_HINT")==0)
    gaHint(msg, cmn, myState);
  else {
    gsResponse(msg, cmn, myState, "MI_ERROR_INVALID_OPCODE", "That is not a valid opCode!");
  }

}

/* 
 * Handles opCode FA_LOCATION
 */
void faLoc(msg_t* msg, communication_t* cmn, state_t* myState) {
  char* thisID = msg->pebbleId;
  if (msg->gameId == 0){
    if (hashtable_find(myState->fieldAgentTable, thisID) == NULL) {
      fa_t* fa = malloc(sizeof(fa_t));
      fa->pebbleId = msg->pebbleId;
      fa->lat = msg->lat;
      fa->lng = msg->lng;
      fa->playerName = msg->playerName;
      fa->playerStatus = true;
      fa->teamName = msg->teamName;
      fa->lastContact = time(NULL);

      faInfo_t* faInfo = malloc(sizeof(faInfo_t));
      faInfo->fa = fa;
      faInfo->cmn = cmn;
      faInfo->captureCode = NULL;
      faInfo->agent = malloc(sizeof(struct sockaddr_in));
      faInfo->socket = initializeSocketClient(cmn->hostName, cmn->port, faInfo->agent);
      faInfo->numCaptures = 0;
      faInfo->numNeutralized = 0;

      fprintf(myState->logFile, "[%s] New player joined. PebbleId=%s, playerName=%s, teamname=%s.\n", currTime(), fa->pebbleId, fa->playerName, fa->teamName);
      //If pebble ID is not in hashtable, increment active players
      if (hashtable_insert(myState->fieldAgentTable, thisID, faInfo)) {
	myState->numActiveFieldAgents++;
	fprintf(myState->logFile, "[%s] Num active agents increased by one. Now %i\n", currTime(), myState->numActiveFieldAgents);
      }
      else {
	faInfo_t* faInfo = hashtable_find(myState->fieldAgentTable, thisID);
	faInfo->cmn = cmn;
	close(faInfo->socket);
	faInfo->agent = malloc(sizeof(struct sockaddr_in));
	faInfo->socket = initializeSocketClient(cmn->hostName, cmn->port, faInfo->agent);
      }
    }
    else {
      faInfo_t* faInfo = hashtable_find(myState->fieldAgentTable, thisID);
      faInfo->cmn = cmn;
      close(faInfo->socket);
      faInfo->agent = malloc(sizeof(struct sockaddr_in));
      faInfo->socket = initializeSocketClient(cmn->hostName, cmn->port, faInfo->agent);
    }
  }
  else if (myState->gameID != msg->gameId){
    //imvalid game ID, need to send a response code here
    gsResponse(msg, cmn, myState, "MI_ERROR_INVALID_GAME_ID", "Invalid game ID!");
    return;
  }
  else {
    faInfo_t* faInfo = hashtable_find(myState->fieldAgentTable, thisID);
    if (faInfo->fa->playerStatus) {
      faInfo->fa->lat = msg->lat;
      faInfo->fa->lng = msg->lng;
    }
    faInfo->fa->lastContact = time(NULL);
  }
  faInfo_t* faInfo = hashtable_find(myState->fieldAgentTable, thisID);
  if (msg->statusReq == 1)
    faStatusReq(msg, faInfo, myState);
}

void faNeutralize(msg_t* msg, communication_t* cmn, state_t* myState) {
  cd_t* drop = list_find(myState->codeDropList, msg->codeId);
  if (drop == NULL)
    return;
  if (drop->neutralizingTeam == NULL || strcmp(drop->neutralizingTeam, "none") != 0)
    return;
  if (withinTen(drop->lat, drop->lng, msg->lat, msg->lng)) {
    drop->neutralizingTeam = msg->teamName;
    gsResponse(msg, cmn, myState, "MI_NEUTRALIZED", "Code drop captured -- suck it, Harvard!");
    myState->numRemainingCodeDrops--;
    faInfo_t* faInfo = hashtable_find(myState->fieldAgentTable, msg->pebbleId);
    if (faInfo != NULL)
      faInfo->numNeutralized++;
  }
}

void faCapture(msg_t* msg, communication_t* cmn, state_t* myState){
  if (strcmp(msg->captureId, "0") == 0) {
    if (time(NULL) - myState->captureTime > 60) {
      myState->captureTime = time(NULL);
      myState->data = (void*) msg;
      hashtable_iterate(myState->fieldAgentTable, generateCaptureCodes, myState);
    }
  }
  else if (myState->captureTime < 60) {
    //Iterate through fa hashtable, checking to see if captureId matches w/ any agents.
    myState->arg1 = msg->captureId;
    myState->arg2 = msg->pebbleId;
    hashtable_iterate(myState->fieldAgentTable, faCaptureHelper, myState);
  }
}

void faCaptureHelper(void* myStateVoid, char* key, void* faInfoVoid) {

  state_t* myState = (state_t*) myStateVoid;
  faInfo_t* faInfo = (faInfo_t*) faInfoVoid;
  if (faInfo->captureCode == NULL)
    return;
  if (strcmp(myState->arg1, faInfo->captureCode)) {
    faInfo->fa->playerStatus = false;

    //TinyMsg just lets gs resonse know that it is talking w/ a field agent, not a guide agent
    msg_t* tinyMsg = malloc(sizeof(msg_t));
    tinyMsg->guideId = 0;
    gsResponse(tinyMsg, faInfo->cmn, myState, "MI_CAPTURED", "Yoooouuu're out!");
    faInfo_t* faInfoCapturer = hashtable_find(myState->fieldAgentTable, myState->arg2);
    if(faInfoCapturer != NULL) {
      gsResponse(tinyMsg, faInfoCapturer->cmn, myState, "MI_CAPTURE_SUCCESS", "Capture complete");
      faInfoCapturer->numCaptures += 1;
    }
    free(tinyMsg);
  }
}

void gaStatus(msg_t* msg, communication_t* cmn, state_t* myState){
  if (msg->gameId == 0){
    if (list_find(myState->guideAgentList, msg->guideId) == NULL) {
      ga_t* ga = malloc(sizeof(ga_t));
      ga->guideID = msg->guideId;
      ga->teamName = msg->teamName;
      ga->lastContact = time(NULL);
      ga->playerName = msg->playerName;

      gaInfo_t* gaInfo = malloc(sizeof(gaInfo_t));
      gaInfo->ga = ga;
      gaInfo->cmn = cmn;
      gaInfo->agent = malloc(sizeof(struct sockaddr_in));
      gaInfo->captureCode = NULL;
      gaInfo->socket = initializeSocketClient(cmn->hostName, cmn->port, gaInfo->agent);
      list_insert(myState->guideAgentList, msg->guideId, gaInfo);
      fprintf(myState->logFile, "[%s] Guide agent added to team %s.\n", currTime(), ga->teamName);
    }
    else {
      gaInfo_t* gaInfo = list_find(myState->guideAgentList, msg->guideId);
      //ga already known, but is reconnecting; update network info
      gaInfo->cmn = cmn;
      close(gaInfo->socket);
      gaInfo->agent = malloc(sizeof(struct sockaddr_in));
      gaInfo->socket = initializeSocketClient(cmn->hostName, cmn->port, gaInfo->agent);
      fprintf(myState->logFile, "[%s] Guide agent recconecting with id %s to team %s.\n", currTime(), msg->guideId, gaInfo->ga->teamName);
      //Ensure team name matches
      if (strcmp(gaInfo->ga->teamName, msg->teamName) != 0) {
	gsResponse(msg, cmn, myState, "MI_ERROR_INVALID_TEAMNAME", "Wrong teamName!");
      }
      gaInfo->ga->lastContact = time(NULL);
    }
  }
  gaInfo_t* gaInfo = list_find(myState->guideAgentList, msg->guideId);
  if (msg->statusReq == 1)
    gaStatusReq(msg, gaInfo, myState);

}

/*
 * Called when the server receives a GA_HINT opCode from a guide agent
 */
void gaHint(msg_t* msg, communication_t* cmn, state_t* myState){
  //validate arguments first
  if (msg->gameId != myState->gameID) {
    gsResponse(msg, cmn, myState, "MI_ERROR_INVALID_GAME_ID", "wrong gameID fool");
    return;
  }
  gaInfo_t* gaInfo = list_find(myState->guideAgentList, msg->guideId);
  if (gaInfo == NULL) { //this shouldnt happen
    return;
  }
  if (strcmp(msg->playerName, gaInfo->ga->playerName) != 0) { //names dont match

    return;
  }
  if (strcmp(msg->guideId, gaInfo->ga->guideID)!=0) { //id's dont match
    gsResponse(msg, cmn, myState, "MI_ERROR_INVALID_ID", "wrong id fool");
    return;
  }
  if (strcmp(msg->pebbleId, "*") == 0) { //send to all field agents

  }
  faInfo_t* faInfo = hashtable_find(myState->fieldAgentTable, msg->pebbleId);
  if (faInfo == NULL) { //unable to find requested fa

  }
  else { //send hint to the faInfo
    char* text = calloc(8192, sizeof(char));
    sprintf(text,"GA_HINT|%d|%s|%s|%s|%s|%s", msg->gameId, msg->guideId, msg->teamName, msg->playerName, msg->pebbleId, msg->message);
    sendToOther(faInfo->socket, faInfo->agent, text);
    free(text);
  }
  gaInfo->ga->lastContact = time(NULL);
}

/*
 * Sends a response code due to an invalid argument
 */
void gsResponse(msg_t* msg, communication_t* cmn, state_t* myState, char* respCode, char* text) {
  char* response = malloc(8192);
  sprintf(response, "GS_RESPONSE|%d|%s|%s", myState->gameID, respCode, text);
  if (msg->guideId == 0) { //must be a field agent
    faInfo_t* faInfo = hashtable_find(myState->fieldAgentTable, msg->pebbleId);
    if (faInfo == NULL) {
      //do nothing?
      free(response);
      return;
    }
    sendToOther(faInfo->socket, faInfo->agent, response);
  }
  else { //must be a guide agent
    gaInfo_t* gaInfo = list_find(myState->guideAgentList, msg->guideId);
    if (gaInfo == NULL) {
      //do nothing?
      free(response);
      return;
    }
    sendToOther(gaInfo->socket, gaInfo->agent, response);
  }
  free(response);
}



/*
 * Implements extra credit to calculate true spherical distance
 */
bool withinTen(float lat1, float lng1, float lat2, float lng2) {
  double xDistance;
  double yDistance;
  double zDistance;
  lng1 -= lng2;
  lng1 *= (3.1415926536 / 180);//convert to radians
  lat1 *= (3.1415926536 / 180);
  lat2 *= (3.1415926536 / 180);

  zDistance = sin(lat1) - sin(lat2); //z distance
  xDistance = cos(lng1) * cos(lat1) - cos(lat2); //x distance
  yDistance = sin(lng1) * cos(lat1); //y distance
  double totalDist = (asin(sqrt((xDistance * xDistance) + (yDistance * yDistance) + (zDistance * zDistance))/ 2) * 2 * 6371) * 1000; //total distance in meters
  return (totalDist < 10);
}

void generateCaptureCodes(void* stateVoid, char* id, void* faInfoVoid) {
  state_t* myState = (state_t*) stateVoid;
  msg_t* msg = (msg_t*) myState->data;
  faInfo_t* faInfo = (faInfo_t*) faInfoVoid;
  fa_t* capturer = hashtable_find(myState->fieldAgentTable,  msg->pebbleId);
  printf("capture");
  fflush(stdout);

  if (capturer == NULL)
    return;
  if (!withinTen(capturer->lat, capturer->lng, faInfo->fa->lat, faInfo->fa->lng))
    return;

  
  char str[] = "0123456789ABCDEF";
  char* code = "";
  srand((unsigned int) time(NULL) + getpid());

  if (faInfo->captureCode != NULL)
    free(faInfo->captureCode);

  for(int i = 0; i<4; i++) {
    strcat(code, &(str[rand() % 16]));
    srand(rand());
  }
  faInfo->captureCode = code;
  sendCaptureMessage(faInfo, myState);
}

void sendCaptureMessage(faInfo_t* faInfo, state_t* myState) {
  char* status = count_malloc_assert(8192, "status msg");

  sprintf(status, "GS_CAPTURE_ID|%i|%s", myState->gameID, faInfo->captureCode);
  if (myState->logRaw)
    fprintf(myState->logFile, "[%s] Message Sent: GS_CAPTURE_ID|%i|%s", currTime(), myState->gameID, faInfo->captureCode);
  sendToOther(faInfo->socket, faInfo->agent, status);
}

void gaStatusReq(msg_t* msg, gaInfo_t* gaInfo, state_t* myState) {

  char* status = count_malloc_assert(8192, "ga status msg");

  myState->cdList = calloc(8192, sizeof(char));
  myState->faList = calloc(8192, sizeof(char));

  //problem in list iterate
  list_iterate(myState->codeDropList, codeDropStringHelper, myState);
  hashtable_iterate(myState->fieldAgentTable, fieldAgentStringHelper, myState);

  if (myState->faList == NULL) {
    if (myState->cdList == NULL) {

      sprintf(status, "GAME_STATUS|%i| | |%ld", myState->gameID, myState->maxTime - time(NULL) + myState->startTime);
      fprintf(myState->logFile, "GAME_STATUS|%i| | |%ld", myState->gameID, myState->maxTime - time(NULL) + myState->startTime);
      fflush(myState->logFile);
    }
    else {
      char formattedCode[sizeof(myState->cdList) + 1];
      strncpy(formattedCode, myState->cdList, sizeof(formattedCode));
      formattedCode[strlen(myState->cdList) - 1] = '\0';
      sprintf(status, "GAME_STATUS|%i| |%s|%ld", myState->gameID, formattedCode, myState->maxTime - time(NULL) + myState->startTime);
      fprintf(myState->logFile, "GAME_STATUS|%i| |%s|%ld", myState->gameID, formattedCode, myState->maxTime - time(NULL) + myState->startTime);
      fflush(myState->logFile);
    }
  }
  else {
    if (myState->cdList == NULL) {
      char formattedList[sizeof(myState->faList) + 1];
      strncpy(formattedList, myState->faList, sizeof(formattedList));
      formattedList[strlen(myState->faList) - 1] = '\0';
      sprintf(status, "GAME_STATUS|%i|%s| |%ld", myState->gameID, formattedList, myState->maxTime - time(NULL) + myState->startTime);

      fprintf(myState->logFile, "GAME_STATUS|%i|%s| |%ld", myState->gameID, formattedList, myState->maxTime - time(NULL) + myState->startTime);
      fflush(myState->logFile);
    }
    else {
      char faFormat[4000];
      char cdFormat[4000];
      strcpy(faFormat, myState->faList);
      strcpy(cdFormat, myState->cdList);
      int len = strlen(myState->faList);
      if (len > 0 && faFormat[len-1] == ':') { //if we need to strip the trailing colon
	faFormat[len-1] = '\0'; //make it a null
      }
      else if (len == 0) {
	faFormat[0] = ' ';
	faFormat[1] = '\0';
      }
      len = strlen(myState->cdList);
      if (len > 0 && cdFormat[len-1] == ':') { //if we need to strip the trailing colon
	cdFormat[len-1]= '\0';//make it a null
      }
      else if (len == 0) {
	cdFormat[0] = ' ';
	cdFormat[1] = '\0';
      }

      sprintf(status, "GAME_STATUS|%i|%s|%s|%ld", myState->gameID, faFormat, cdFormat, myState->maxTime - time(NULL) + myState->startTime);
      fprintf(myState->logFile, "GAME_STATUS|%i|%s|%s|%ld", myState->gameID, faFormat, cdFormat, myState->maxTime - time(NULL) + myState->startTime);
      fflush(myState->logFile);
    }
  }
  //send the message
  sendToOther(gaInfo->socket, gaInfo->agent, status);

  free(status);
  //cleanup free'd memory
  free(myState->cdList);
  free(myState->faList);
}

void findGuideByTeam(void* arg, char* key, void* data)
{
  state_t* myState = (state_t*) arg;
  gaInfo_t* gaInfo = (gaInfo_t*) data;
  if (strcmp(myState->arg1, gaInfo->ga->teamName) == 0) { //found a match
    myState->data = gaInfo;
  }
}

void faStatusReq(msg_t* msg, faInfo_t* faInfo, state_t* myState) {
  char* status = count_malloc_assert(8192, "status msg");

  //call iterator to find the correct guide agent
  myState->arg1 = faInfo->fa->teamName;
  list_iterate(myState->guideAgentList, findGuideByTeam, myState);

  gaInfo_t* guide = (gaInfo_t*) myState->data;
  char* guideID = calloc(10, sizeof(char));
  if (guide == NULL){
    strcpy(guideID, "0");
  }
  else guideID = guide->ga->guideID;
  myState->teamName = faInfo->fa->teamName;
  myState->numFriends = 0;
  hashtable_iterate(myState->fieldAgentTable, numFriendlyAgents, myState);
  int numFriends = myState->numFriends;

  sprintf(status, "GAME_STATUS|%i|%s|%i|%i|%i", myState->gameID, guideID, myState->numRemainingCodeDrops, numFriends, myState->numActiveFieldAgents - numFriends);
  if (myState->logRaw)
    fprintf(myState->logFile, "[%s] Message Sent: GAME_STATUS|%i|%s|%i|%i|%i", currTime(), myState->gameID, guideID, myState->numRemainingCodeDrops, numFriends, myState->numActiveFieldAgents - numFriends);
  sendToOther(faInfo->socket, faInfo->agent, status);
}

//Changes the value of numFriends to the number of active Agents friendly to fa

void numFriendlyAgents(void* myStateVoid, char* key, void* faInfoVoid) {
  state_t* myState = (state_t*) myStateVoid;
  faInfo_t* faInfo = (faInfo_t*) faInfoVoid;
  if (strcmp(myState->teamName, faInfo->fa->teamName) == 0 && faInfo->fa->playerStatus)
    myState->numFriends++;
}



//populates myState->cdList w/ properly formatted list of code drops for status req
void codeDropStringHelper(void* myStateVoid, char* key, void* cdVoid) {
  state_t* myState = (state_t*) myStateVoid;
  cd_t* cd = (cd_t*) cdVoid;
  char* str = malloc(sizeof(char) * 200);
  sprintf(str, "%s,%f,%f,%s:", cd->hexCode, cd->lat, cd->lng, cd->neutralizingTeam);
  strcat(myState->cdList, str);
  //free(str);
}
//populates myState->faList w/ properly formatted list of field agents for status req
void fieldAgentStringHelper(void* myStateVoid, char* key, void* faInfoVoid) {
  faInfo_t* faInfo = (faInfo_t*) faInfoVoid;
  state_t* myState = (state_t*) myStateVoid;
  char* str = malloc(sizeof(char) * 200);
  char* active = malloc(10);
  if (faInfo->fa->playerStatus)
    sprintf(active, "true");
  else sprintf(active, "false");

  sprintf(str, "%s,%s,%s,%s,%f,%f,%li:", faInfo->fa->pebbleId, faInfo->fa->teamName, faInfo->fa->playerName, active, faInfo->fa->lat, faInfo->fa->lng, faInfo->fa->lastContact);
  strcat(myState->faList, str);

  //free(active);
  //free(str);

}

void gameOver(state_t* myState) {
  //List keyed by teamname, containing listStats_t* struct ptrs
  list_t* statsList = list_new();
  myState->statsList = statsList;
  hashtable_iterate(myState->fieldAgentTable, gameOverMsgField, myState->statsList);
  list_iterate(myState->guideAgentList, gameOverMsgGuide, myState->statsList);

  myState->arg1 = calloc(8192, sizeof(char));

  list_iterate(myState->statsList, generateStatsString, myState);


  int len = strlen(myState->arg1);
  if (len > 0 && myState->arg1[len-1] == ':') { //if we need to strip the trailing colon
    myState->arg1[len-1]= '\0';//make it a null
  }
  else if (len == 0) {
    myState->arg1[0] = ' ';
    myState->arg1[1] = '\0';
  }


  hashtable_iterate(myState->fieldAgentTable, gameOverMsgFieldSend, myState);
  list_iterate(myState->guideAgentList, gameOverMsgGuideSend, myState);
}

void gameOverMsgField(void* statsListVoid, char* key, void* faInfoVoid) {
  list_t* statsList = (list_t*) statsListVoid;
  faInfo_t* faInfo = (faInfo_t*) faInfoVoid;
  teamStats_t* teamStats = list_find(statsList, faInfo->fa->teamName);
  if (teamStats == NULL) {
    teamStats = malloc(sizeof(teamStats_t));
    teamStats->teamName = faInfo->fa->teamName;
    teamStats->numPlayers = 0;
    teamStats->numCaptures = 0;
    teamStats->numCaptured = 0;
    teamStats->numNeutralized = 0;
    list_insert(statsList, teamStats->teamName, teamStats);
  }

  teamStats->numPlayers += 1;
  teamStats->numCaptures += faInfo->numCaptures;
  if (!faInfo->fa->playerStatus)
    teamStats->numCaptured += 1;
  teamStats->numNeutralized += faInfo->numNeutralized;
}

void gameOverMsgGuide(void* statsListVoid, char* key, void* gaInfoVoid) {
  list_t* statsList = (list_t*) statsListVoid;
  gaInfo_t* gaInfo = (gaInfo_t*) gaInfoVoid;
  teamStats_t* teamStats = list_find(statsList, gaInfo->ga->teamName);
  if (teamStats == NULL) {
    teamStats = malloc(sizeof(teamStats_t));
    teamStats->teamName = gaInfo->ga->teamName;
    teamStats->numPlayers = 0;
    teamStats->numCaptures = 0;
    teamStats->numCaptured = 0;
    teamStats->numNeutralized = 0;
    list_insert(statsList, teamStats->teamName, teamStats);
  }
  teamStats->numPlayers += 1;
}

//Populates myState->arg1 w/ stats
void generateStatsString(void* myStateVoid, char* key, void* teamStatsVoid){
  state_t* myState = (state_t*) myStateVoid;
  teamStats_t* teamStats = (teamStats_t*) teamStatsVoid;

  char* str = malloc(sizeof(char) * 200);

  sprintf(str, "%s,%i,%i,%i,%i:", teamStats->teamName, teamStats->numPlayers, teamStats->numCaptures, teamStats->numCaptured, teamStats->numNeutralized);
  strcat(myState->arg1, str);
}


void gameOverMsgGuideSend(void* myStateVoid, char* key, void* gaInfoVoid) {
  state_t* myState = (state_t*) myStateVoid;
  gaInfo_t* gaInfo = (gaInfo_t*) gaInfoVoid;
  char* stats = count_malloc_assert(8192, "status msg");

  sprintf(stats, "GAME_OVER|%i|%i|%s", myState->gameID, myState->numRemainingCodeDrops, myState->arg1);
  if (myState->logRaw)
    fprintf(myState->logFile, "GAME_OVER|%i|%i|%s", myState->gameID, myState->numRemainingCodeDrops, myState->arg1);
  sendToOther(gaInfo->socket, gaInfo->agent, stats);


}

void gameOverMsgFieldSend(void* myStateVoid, char* key, void* faInfoVoid) {
  state_t* myState = (state_t*) myStateVoid;
  faInfo_t* faInfo = (faInfo_t*) faInfoVoid;
  char* stats = count_malloc_assert(8192, "status msg");

  sprintf(stats, "GAME_OVER|%i|%i|%s", myState->gameID, myState->numRemainingCodeDrops, myState->arg1);
  if (myState->logRaw)
    fprintf(myState->logFile, "GAME_OVER|%i|%i|%s", myState->gameID, myState->numRemainingCodeDrops, myState->arg1);
  sendToOther(faInfo->socket, faInfo->agent, stats);


}


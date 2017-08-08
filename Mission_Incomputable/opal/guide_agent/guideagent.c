/* guideagent.c - main module that runs the guide agent
 * Chris Bertasi, Andrew Laub, Michael Kilgore                                                            
 * May, 2016                                                                                              
 * For use in CS50 final project
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../lib/memory/memory.h"
#include "../commonz/socket.h"
#include "../commonz/message.h"
#include <time.h>
#include "../lib/list/list.h"
#include "../lib/bag/bag.h"

//** Function Declarations **//
void parseArgs(int argc, char* argv[], state_t* myState);
void sendStatusRequest(state_t* myState, int statusReq);
void sendHint(state_t* myState, char* pebbleID, char* hint);
void handleStdin(char* input, state_t* myState);
void handleSocketInput(communication_t* cmn, state_t* myState);
void updateGameStatus(msg_t* msg, state_t* myState);
void gameOver(msg_t* msg, state_t* myState);
void gsResponse(msg_t* msg, state_t* myState);
static char* getTime();
static void printTeam(void* arg, char* key, void* data);
static void hintIterate(void* arg, char* key, void* data);
static void printGame(state_t* myState);
static void printCodeIterate(void* arg, char* key, void* data);
static void printAgentIterate(void* arg, char* key, void* data);
static bool isHex(char* hex);
static void fieldAgentDelete(void* arg, char* key, void* data);
static void codeDropDelete(void* arg, char* key, void* data);

/*
 * main func
 */
int main(int argc, char* argv[])
{
  //declare and malloc space for variables used in parseArgs

  state_t* myState = count_malloc_assert(sizeof(state_t), "myState");
  myState->server = count_malloc_assert(sizeof(struct sockaddr_in), "myState->server");
  myState->gameRunning = true;
  myState->logRaw = false;
  myState->gameID = 0;
  myState->guideID = NULL;
  
  myState->logFile = fopen("../log/guideagent.log", "a"); //open the log file for appending
  if (myState->logFile == NULL) {
    fprintf(stderr, "Unable to open the log file!\n");
    exit(5);
  }
  
  
  //this is the time used to wait before sending a request to the gameServer
  const int DIFFTIME = 5; //**SHOULD BE 15 IN ACTUAL GAMEPLAY, LOWER FOR DEBUGGING**//
  
  parseArgs(argc, argv, myState);
  
  myState->socket = initializeSocketClient(myState->gsHost, myState->gsPort, myState->server);
  fprintf(myState->logFile, "[%s] Initialized socket to server %s at port %d\n", getTime(), myState->gsHost, myState->gsPort);
  
  myState->codeDropList = list_new();
  myState->fieldAgentList = list_new();
  
  
  time_t time1 = time(NULL);
  while (myState->gameRunning) {
    checkForInput(myState, handleStdin, handleSocketInput);
    if (difftime(time(NULL), time1) > DIFFTIME) {
      sendStatusRequest(myState, 1);
      time1 = time(NULL);
      fflush(myState->logFile); //so we actually print to logFile
    }
  }
  fflush(stdout);
  printf("Logging off...\n");
  fprintf(myState->logFile, "[%s] Logging off...\n", getTime());
  fprintf(myState->logFile, "-----------------------------------------------------------\n\n\n");
  
  fflush(myState->logFile);
  fclose(myState->logFile);
  close(myState->socket);
  list_delete(myState->fieldAgentList);
  list_delete(myState->codeDropList);
  free(myState->server);
  free(myState->guideID);
  free(myState);
  return 0;
}

/*
 * Handles input received from a socket
 * called by a function pointer sent to socket module
 */
void handleSocketInput(communication_t* cmn, state_t* myState)
{
  fprintf(myState->logFile, "[%s] Received message from %s@%d:\n", getTime(), cmn->hostName, cmn->port);
  if (myState->logRaw) { //log everything
    fprintf(myState->logFile, "[%s] %s\n", getTime(), cmn->msg);
  }
  msg_t* msg = message_parse(cmn->msg);
  if (strcmp(msg->opCode, "GAME_STATUS") == 0) {
    updateGameStatus(msg, myState);
  }
  else if (strcmp(msg->opCode, "GAME_OVER") == 0) {
    gameOver(msg, myState);
  }
  else if (strcmp(msg->opCode, "GS_RESPONSE") == 0) {
    gsResponse(msg, myState);
  }
  else { //bad opCode
    fprintf(myState->logFile, "[%s] Received bad opCode: %s\n", getTime(), msg->opCode);  
  }
  message_delete(msg);
}

/*
 * Updates the game status upon receiving from the server
 */
void updateGameStatus(msg_t* msg, state_t* myState)
{
  if (myState->gameID == 0) {
    myState->gameID = msg->gameId;
    fprintf(myState->logFile, "[%s] Received initial message from server, gameID is now %d\n", getTime(), myState->gameID);
    printf("Connected and received info from the game server!\n");
  }
  else if (myState->gameID != msg->gameId) { //wrong game?
    fprintf(myState->logFile, "[%s] Received message from server for wrong game: ", getTime());
    fprintf(myState->logFile, "expected game id of %d, but received id of %d\n", myState->gameID, msg->gameId);
    return;
  } else { //gameID is correct
    fprintf(myState->logFile, "[%s] Received message from server\n", getTime());
  }
  //free old allocated memory
  list_iterate(myState->fieldAgentList, fieldAgentDelete, myState); //delete each individual fa
  list_iterate(myState->codeDropList, codeDropDelete, myState); //delete each individual cd
  list_delete(myState->fieldAgentList);
  list_delete(myState->codeDropList);

  //update my game state
  myState->fieldAgentList = msg->faList;
  myState->codeDropList = msg->cdList;
  myState->timeLeft = msg->timeLeft;
  
  
}

//used by the iterator to delete cd records
static void codeDropDelete(void* arg, char* key, void* data)
{ 
  cd_t* cd = (cd_t*) data;
  free(cd->hexCode);
  free(cd->neutralizingTeam);
  free(cd);
}



//used by the iterator to delete fa records
static void fieldAgentDelete(void* arg, char* key, void* data)
{
  fa_t* fa = (fa_t*) data;
  free(fa->pebbleId);
  free(fa->playerName);
  free(fa->teamName);
  free(fa);
}


/*
 * Called when the game server sends a GAME_OVER message
 * prints out relevant info
 */
void gameOver(msg_t* msg, state_t* myState)
{
  if (myState->gameID != msg->gameId) {
    fprintf(myState->logFile, "Received invalid gameOver message\n");
    fprintf(myState->logFile, "Expected gameID of %d, but received gameID of %d\n", myState->gameID, msg->gameId);
    return;
  }
  myState->gameRunning = false; //update the global flag to stop the game
  printf("The game has now ended!\n");
  printf("Printing results of the game:\n");
  printf("----------------------------------------------\n");
  printf("There are a total of %d remaining code drops!\n\n", msg->numRemainingCodeDrops);
  list_t* teams = msg->team_record_list;
  list_iterate(teams, printTeam, myState);
  printf("----------------------------------------------\n");
  printf("Thanks for playing!\n");
  fprintf(myState->logFile, "[%s] Game ended\n", getTime());
  
}

/*
 * static helper method to print a team from a list_iterate
 */
static void printTeam(void* arg, char* key, void* data)
{
  state_t* myState = (state_t*) arg;
  team_t* team = (team_t*) data; //get the team
  fprintf(myState->logFile, "[%s] Printing game information for team %s\n", getTime(), team->teamName);
  printf("\nTeam %s:\n", team->teamName);
  printf("had a total of %d players throughout the game,\n", team->numPlayers);
  printf("captured a total of %d enemy players,\n", team->numCaptures);
  printf("had %d of their own players captured by enemies,", team->numCaptured);
  printf("and neutralized %d code drops!\n\n", team->numNeutralized);
  
  free(team->teamName);
  free(team);
}

/*
 * Returned when the game server got an unexpected message
 */
void gsResponse(msg_t* msg, state_t* myState)
{
  if (strcmp(msg->respCode, "MI_ERROR_INVALID_GAME_ID") == 0) {
    fprintf(myState->logFile, "[%s] Received MI_ERROR_INVALID_GAME_ID from server\n", getTime());
    printf("Connected to game server, but the guide agent is playing with an incorrect game id.\n");
    printf("Please try reconnecting to this server if you would like to play.\n");
    myState->gameRunning = false;
    return;
  }
  if (myState->gameID != msg->gameId) {
    fprintf(myState->logFile, "[%s] Received invalid response code message\n", getTime());
    fprintf(myState->logFile, "[%s] Expected gameID of %d, but received gameID of %d\n", getTime(), myState->gameID, msg->gameId);
    return;
  }
  if (strcmp(msg->respCode, "MI_ERROR_INVALID_OPCODE") == 0) {
    fprintf(myState->logFile, "[%s] Received MI_ERROR_INVALID_OPCODE from server\n", getTime());
  } else if (strcmp(msg->respCode, "MI_ERROR_INVALID_TEAMNAME") == 0) {
    fprintf(myState->logFile, "[%s] Received MI_ERROR_INVALID_TEAMNAME from server\n", getTime());
    myState->gameRunning = false;
    printf("The teamname %s is taken, please connect with a new teamname!\n", myState->teamName);
    //need to change team name
  } else if (strcmp(msg->respCode, "MI_ERROR_INVALID_ID") == 0) {
    fprintf(myState->logFile, "[%s] Received MI_ERROR_INVALID_ID from server\n", getTime());
    //need to change id
    myState->gameRunning = false;
    printf("The guide id %s is taken, please connect with a new guide id!\n", myState->guideID);
  } else { //received unknown response code
    fprintf(myState->logFile, "[%s] Received unknown response code: %s from the server\n", getTime(), msg->respCode);
  }
  
}

/*
 * Validates input received from stdin
 * this function is called as a function pointer by socket module
 */
void handleStdin(char* input, state_t* myState)
{
  if (strcmp(input, "help") == 0) {
    printf("Commands: \n");
    printf("map - opens a GTK map as a graphical display\n");
    printf("hint fieldAgentHexCode message - send a hint that is 140 characters or less to the specified field agent (use * to send to all agents)\n");
    printf("print - prints out game state information\n");
    printf("logout - exit the program\n");
    return;
  }
  if (strcmp(input, "map") == 0) {
    //**OPEN THE GTK MAP HERE**//
    printf("Opening the GTK map...\n");
    return;
  }
  if (strcmp(input, "print") == 0) {//print out useful info about game
    printGame(myState);
    return;
  }
  if (strcmp(input, "logout") == 0) {
    myState->gameRunning = false;
    return;
  }
  char* token = strtok(input, " ");
  if (token == NULL) {
    return;
  }
  if (strcmp(token, "hint") == 0) {
    myState->sentHint = false;
    token = strtok(NULL, " ");
    if (token == NULL) {
      printf("Usage: hint fieldAgentHexCode message\n");
      return;
    }
    if (strcmp(token, "*") == 0) {
      token = strtok(NULL, "");
      if (token == NULL) {
	printf("Usage: hint fieldAgentHexCode message\n");
	return;
      }
      int len = strlen(token);
      if (len > 140) {
	printf("The hint message length must be less than or equal to 140 characters!\n");
	return;
      }
      printf("Sending hint '%s' to all field agents...\n", token);
      myState->arg1 = "*"; //agent were looking for (all agents)
      myState->arg2 = token; //holds the hint message
      list_iterate(myState->fieldAgentList, hintIterate, myState);
      if (myState->sentHint == false) {
	printf("No friendly field agents to send the hint to!\n");
      }
      
      return;
    }
    else {
      char* agentHex = token;
      token = strtok(NULL, "");
      if (token == NULL) {
	printf("Usage: hint fieldAgentHexCode message\n");
	return;
      }
      int len = strlen(token);
      if (len > 140) {
	printf("The hint message length must be less than or equal to 140 characters!\n");
	return;
      }
      
      myState->arg1 = agentHex; //holds hexcode of agent were looking for
      myState->arg2 = token; //holds the hint message
      list_iterate(myState->fieldAgentList, hintIterate, myState);
      if (myState->sentHint == false) {
	printf("Unable to find agent %s, the hint could not be sent!\n", agentHex);
      }
    }
  }
}

/*
 * Prints out useful information about the game
 */
static void printGame(state_t* myState)
{
  fprintf(myState->logFile, "[%s] Printing out the current game info to stdout...\n", getTime());
  printf("\n---------Current Game Status--------\n\n");
  printf("|Code Drop Status|\n\n");
  list_iterate(myState->codeDropList, printCodeIterate, myState);
  printf("\n|%s Team Field Agent Status|\n\n", myState->teamName);
  list_iterate(myState->fieldAgentList, printAgentIterate, myState);
  printf("\n|Time Remaining: %ld seconds|\n\n", myState->timeLeft);
  printf("------------------------------------\n");
}

/*
 * Called by iterator to print out info about each friendly field agent
 */
static void printAgentIterate(void* arg, char* key, void* data)
{
  fa_t* fa = (fa_t*) data;
  state_t* myState = (state_t*) arg;

  //first validate that this agent is on my team
  if (fa->teamName == NULL) {
    return;
  }
  if (strcmp(myState->teamName, fa->teamName) != 0) { //wrong team
    return;
  }
  
  printf("|Field Agent| AgentName:%s PebbleID:%s",fa->playerName, fa->pebbleId);
  printf(" IsActive:%s", fa->playerStatus ? "True":"False");
  printf(" Lat:%f Long:%f", fa->lat, fa->lng);
  long int now = time(NULL);
  int secondsSince = (int) now - fa->lastContact;
  printf(" LastContact:%d seconds\n", secondsSince);
}


/*
 * Called by iterator to print out info about each code drop
 */
static void printCodeIterate(void* arg, char* key, void* data)
{
  cd_t* codeDrop = (cd_t*) data;
  //state_t* myState = (state_t*) arg;
  printf("|Code Drop| lat:%f long:%f capturedBy:%s\n", codeDrop->lat, codeDrop->lng, codeDrop->neutralizingTeam);
  
}

/*
 * Sends a hint to all matching friendly field agents
 */
static void hintIterate(void* arg, char* key, void* data)
{
  fa_t* agent = (fa_t*) data;
  state_t* myState = (state_t*) arg;
  if (strcmp(myState->teamName, agent->teamName) != 0) { //looking for friendly agents only
    return;
  }
  char* targetHex = myState->arg1;
  if (strcmp(targetHex, "*") == 0 || strcmp(targetHex, agent->pebbleId) == 0) {
    //we have a match, send a hint
    char* message = myState->arg2;
    printf("Sending agent %s (name: %s) the following hint: %s\n", targetHex, agent->playerName, message);
    sendHint(myState, agent->pebbleId, message);
    myState->sentHint = true;
  }
}


/*
 * Sends a GA_STATUS message to the game server
 */
void sendStatusRequest(state_t* myState, int statusReq)
{
  char* msg = count_malloc_assert(8192, "status message");
  //sprintf(msg, "FA_LOCATION|%d|1000|%s|%s|111.111|222.222|%d", myState->gameID, myState->teamName, myState->playerName, statusReq);
  //used for debugging with game server
  
  sprintf(msg, "GA_STATUS|%d|%s|%s|%s|%d", myState->gameID, myState->guideID, myState->teamName, myState->playerName, statusReq);
  sendToOther(myState->socket, myState->server, msg);
  fprintf(myState->logFile, "[%s] Sending GA_STATUS request to the server...\n", getTime());
  if (myState->logRaw) {
    fprintf(myState->logFile, "[%s] %s\n", getTime(), msg);
  }
  count_free(msg);
}

/*
 * Sends a GA_HINT message to the game server
 */
void sendHint(state_t* myState, char* pebbleID, char* hint)
{
  char* msg = count_malloc_assert(8192, "hint message");
  sprintf(msg, "GA_HINT|%d|%s|%s|%s|%s|%s", myState->gameID, myState->guideID, myState->teamName, myState->playerName, pebbleID, hint);
  sendToOther(myState->socket, myState->server, msg);
  fprintf(myState->logFile, "[%s] Sending GA_HINT request to the server...\n", getTime());
  if (myState->logRaw) {
    fprintf(myState->logFile, "[%s] %s\n", getTime(), msg);
  }
  count_free(msg);
}


void parseArgs(int argc, char* argv[], state_t* myState)
{
  bag_t* switches = bag_new();
  int switchCount = 0;
  for (int i = 1; i < argc; i++) { //start at one to ignore the command itself
    if (argv[i][0] == '-') { //it's a switch!
      bag_insert(switches, argv[i]);
      switchCount++;
    }
  } //we now hold all the switches
  if ((argc - switchCount) != 5) {
    fprintf(stderr, "Incorrect number of arguments!\n");
    fprintf(stderr, "Usage: ./guideagent [-v] [-id=########] teamName playerName GShost GSport\n");
    exit(2);
  }
  for (int i = 0; i < switchCount; i++) {
    char* arg = bag_extract(switches);
    if (strcmp(arg, "-v") == 0) {
      myState->logRaw = true;
    }
    else {
      int len = strlen(arg);
      if (len == 12) { //should be (hopefully) -id=########
	if (arg[0] == '-' && arg[1] == 'i' && arg[2] == 'd' && arg[3] == '=') {
	  char* code = malloc(sizeof(char) * 12);
	  int spot = 0;
	  for (int y = 4; y < 12; y++) {
	    code[spot] = arg[y];
	    spot++;
	  }
	  code[8] = '\0';
	  if (isHex(code)) { //they passed in a valid guideID
	    myState->guideID = code;
	  }
	  else {
	    fprintf(stderr, "Invalid hexCode:%s for guideID, will generate a random code instead\n", code);
	    fprintf(myState->logFile, "User entered invalid hex code, will generate a rancom code instead\n");
	    free(code);
	  }	  
	}
	else { //unrecognized switch
	  fprintf(stderr, "Unrecognized switch: %s\n", arg);
	}
      }
      else { //unrecognized switch
	fprintf(stderr, "Unrecognized switch: %s\n", arg);
      }
    }
  } //done dealing with switches
  myState->teamName = argv[1 + switchCount];
  myState->playerName = argv[2 + switchCount];
  myState->gsHost = argv[3 + switchCount];
  myState->gsPort = (int) strtol(argv[4 + switchCount], NULL, 10);
  
  //check if guide id is initialized to a valid id
  if (myState->guideID == NULL) {
    srand(time(NULL));
    char str[] = "0123456789ABCDEF";
    myState->guideID = malloc(sizeof(char) * 12);
    for(int i = 0; i < 8; i++) {
      myState->guideID[i] = str[rand() % 16];
    }
    myState->guideID[8] = '\0';
  }
  bag_delete(switches, NULL);

  fprintf(myState->logFile, "-----------------------------------------------------------\n");
  fprintf(myState->logFile, "[%s] Successfully initialized a new guide agent\n", getTime());
  fprintf(myState->logFile, "[%s] After validating command line arguments with %d switches, we have:\n", getTime(), switchCount);
  fprintf(myState->logFile, "teamName is %s, playerName is %s, guideID is %s, gsHost is %s, gsPort is %d, logRaw is %d\n", myState->teamName, myState->playerName, myState->guideID, myState->gsHost, myState->gsPort, myState->logRaw);

}

/*
 * Validates that a passed in string is in fact a hex code
 */
static bool isHex(char* hex)
{
  char str[] = "0123456789ABCDEF";
  int len = strlen(hex);
  for (int i = 0; i < len; i++) {
    bool goodChar = false;
    for (int x = 0; x < 15; x++) {
      if (hex[i] == str[x]) { //matches a hex char
	goodChar = true;
	break;
      }
    }
    if (goodChar == false) {
      return false;
    }
  }
  return true; 
}

	  
/*
 * Used to get the current time nicely formatted
 * very handy for use in the log function
 */
static char* getTime()
{
  long int startTime = time(NULL);
  struct tm* local = localtime(&startTime);
  char* time = asctime(local);
  strtok(time, "\n");
  return time;
}


# Implementation Specs for Mission Incomputable - Opal Team
### _Chris Bertasi, Andrew Laub, Michael Kilgore_

****_Please see flow.png for visual diagram of modules_**

****_Please see Field_Agent_GUI.jpeg for a visual representation of the window scheme_**

## Game Server
Modules include:
* socket handler
* message parser
* main
  * contains data structures from previous lab
  * contains structures for a field agent and a guide agent
  * we anticipate holding the guide agents in a list
  * we anticipate holding field agents in a hash table of lists of field agents, key'd by teamName
  * we anticipate holding code drops in a structure that has lat, long, and hex code
  * the code drops will be held in a list

### Main Module

Structs:
* Field Agent:
  * int - pebbleID
  * float - lastKnownLat
  * float - lastKnownLong
  * char* - playerName
  * bool - playerStatus (true = active, false = captive)
  * char* - teamName
  * int - secondsSinceLastContact
* Guide Agent:
  * int - gameID
  * int - guideID
  * char* teamName
  * long - lastContact
* Code Drop:
  * float - lat
  * float - long
  * int - hexCode
  * char* - neutralizingTeam ("NONE" if not yet captured)
* Message:
  * int - opCode
  * int - gameID
  * int - guideID
  * char* - teamName
  * char* - playerName
  * int - statusReq
  * int - pebbleID
  * char* - message
  * int - numRemainingCodeDrops
  * int - numFriendlyOperatives
  * int - numFoeOperatives
  * int - captureID
  * list - fieldAgentList (stores field agent structs)
  * list - codeDropList (stores code drop structs)
* previous lab structs

Psuedo Code:
* validate command line arguments
  * optionally initialize a log file
* read in from file that has code drop information
  * the code drops will be added to a list of code drops
* create a DGRAM (UDP) socket to receive incoming data
  * socket will bind to a command line port number
* initialize a random game number
* listen on socket for incoming DGRAMS
  * pass all incoming messages to the parsing module
  * the parsing module will return information (in the form of a message struct) from the datagram about who has contacted the server and the appropriate game information
  * if a new guide agent, the server will add it to a list of known guide agents (and teams)
  * field agents will not be able to join the game until a guide agent of the same team has entered (these messages will be ignored)
  * guide agents connecting for the first time with a non-unique ID will be ignored
  * the server will listen for game status requests from guide agents, and will respond with the game state when prompted by guide agents
  * the server will listen for messages from the field agents
    * upon receiving a message, the parser will validate all passed in arguments
      * if the arguments fail to validate, the information will be ignored
    * if the OP code is FA_LOCATION, the server will update the field agents locations
      * if statusReq is 1, the server will send a gameStatus message back to the field agent
    * if the OP code is  FA_NEUTRALIZE:
      * the server will first validate that the players lat and long are within 10 meters of the code drop's lat and long
      * the server will validate that the 4 digit hex code is correct, and if it is, remove the code drop from the list of code drops
        * the guide agent struct for that team will then increment an integer holding that teams score
	* the server will then send a message to the field agent that they successfully captured a code drop
    * if the OP code is FA_CAPTURE:
      * if the captureID is 0:
        * for each nearby (within 10m) Field Agent (from a different team), generate a random captureId and send it to that Field Agent in a GS_CAPTURE_ID message
      * if the captureID is non-0:
        * confirm that the captureId matches the captureId provided within the past 60s in response to this Field Agent’s request to capture
	* if confirmed, mark the Field Agent as captured, and update a variable inside the Field Agent's struct to mark it captured (all future messages from that agent will be ignored)
	* send a MI_CAPTURE_SUCCESS message to the capturing Field Agent
  * the server will also listen for messages from Guide Agents:
    * if the OP code is GA_STATUS:
      * the server will validate the gameId, guideId, teamName, and playerName
      * if gameId==0, register player guideId, teamName, playerName.
        * if the guideId is not yet known, add a new player.
        * if the guideId is known, verify that teamName matches its known teamName
      * if statusReq is 1:
        * the server will send out a game status update to the guide agent
      * the server will update the last known contact time of the guide agent (contained in each GA struct)
    * if the OP code is GA_HINT
      * the server will validate the gameId, guideId, teamName, and playerName (in the message parser module)
      * if playerName is *, the message argument will be sent to all of that teams field agents
      * if the playerName is a known field agent name, the message will be sent to that specific field agent
      * update records regarding last-contact-time of this Guide Agent.

### Map module
* upon calling the map module, a gtk+ window appears with a scrollable map of campus
  * periodically (every 15 seconds), refresh the contents of the map
    * add location of each remaining code drop and field agent, organized by color


### Socket Handler Module
Pseudo Code:
* called by the main function to initialize a DGRAM socket
* set up a socket to receive messages using AF_INET and DGRAM
* if unable to initialize a socket, exit with non-zero status code
* bind the socket to the specified port
  * if unable to bind to the port, exit with a non-zero status code
* return the initialized socket back to the main module

### Message Parser Module
Structs:
* Message:
  * see above struct
* Field Agent:
  * see above struct
* Code Drop:
  * see above struct

Pseudo Code:
* called by the main function after receiving a message
* begin by counting the number of |'s in the message
* create an array of char*'s of that length
* use open-source strtrok to break each field up and insert them in to the array
* initialize a message struct
* fill in the appropriate fields based on the opCode
* if the message is malformmatted/the fields are of wrong type, return null
* return the formmatted message struct (the main method will then validate the values)

## Guide Agent
Module:
* message parser module
* socket module
* main

Structs:
* Field agent
  * see above struct
* Code Drop:
  * see above struct


Pseudo Code:
* validate command line arguments
  * optionally initialize a log file (based on command line arguments)
* use the socket module to create a new socket for a random port
  * if this fails, keep trying with a new random ports
* upon success, send a connect message (GA_STATUS) to the game server w/ a game number of 0, and a request status of 1
* listen for information about the current game, and set a variable in the main function for the game number
* request periodic (every 15 seconds) game status updates by sending GA_STATUS w/ a request status of 1, and a non-zero game number
* upon recieving from stdin
  * if the `map` command is given, open the map
  * otherwise, command is a hint, and we must ensure it is under 140 characters
  * If so, pass the message to the server in proper format (using GA_HINT)
  * Otherwise, notify the user that the message was too long, and continue main loop
  

### Map module
* the map module will be a watered down version of the game server map module (will only friendly show agent locations)


## Field Agent

Modules:

* Main window module
* Select team name and agent name window
* Enter capture code/neutralize window
* View game messages window
* Message parsing module

Structs:
* Message module from above

### Main Window
Pseudo Code:

* Aop event loop
  * setup select button listener
  * wait select button to be pushed on specific menu item in main window
  * set up inbox to receive messages from game server
    * on message received, update game status
  * set up message sender to build and send messages to game server
    * when game message is received, parse message and update game status
  * set up accelarometer listener
    * on movement registered send message to server with new lat:lng

  * display menu options: "Select team name and agent name," "Capture,"Neutralize," "Go active," "Game messages"
    * if item selected is "select agent and team name"
      * push team name and agent name window onto window stack
      * listen for team name or agent name changes and set those variables
    * if item selected is "Capture"
      * if not is_capturing
        * send message to game server with captureid of zero
        * set boolean is_capturing to true
      * Push capture code/neutralize window
    * if item selected is "Neutralize"
      * Push capture code/neutralize window
    * if item selected is "Go active"
      * Check if is_active is true
      	* if false
          * set is_active boolean to true
          * send message to game server with gameId=0, pebbleID, team name, player name, lat:lng, statusReq =1
          * process message sent to inbox with message parsing module
    * if item selected is "Game messages"
      * Push game messages window
    
### Select team name and agent name window
Pseudo Code:
* display predetermined team names and agent names in separate menu layer sections
* set up select button listener
* if team name is selected, report back selection to main module
* if agent name is selected, report back selection to main module
* on back pressed, pop window off stack and destroy window

### Enter capture code/neutralize window
Pseudo Code:
* display 4-digit keypad to enter hex code
* set up listener for the submit of the 4-digit code
* if code is submitted
  * send message to game server with opcode FA_NEUTRALIZE and arguments gameId, pebbleId, teamname, PlayerName, lat, lng, codeId(where the code Id is the entered 4-digit code)

### View Game Messages window
Pseudo Code:
* set up scroll layer with text layer inside
* display string of game messages in the game status struct inside the main module

<<<<<<< HEAD
*_Please see Field_Agent_GUI.jpeg for a visual representation of the window scheme_*
=======
### Field agent GUI
* please see docs/gui.jpg


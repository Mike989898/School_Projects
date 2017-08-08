# README file for guideagent.c
## Chris Bertas, Andrew Laub, Michael Kilgore
## May 2016


## Compiling
   make

## Usage
   Usage: ./guideagent [-v] [-id=########] teamName playerName GShost GSport
* [-v] : optional switch to enable verbose logging
* [id=########] : optional switch to manually specifiy the guide id (if not given, randomly generates)
* teamName : the team that you would like to join
* playerName : your guide agent name
* GShost : the hostname of the game server to connect to
* GSport : the port of the game server to connect to

## Command Line Inputs
* help : prints out a list of valid command line inputs
* logout : exits the program
* print : prints out relevant game state information
* hint fieldAgentHexCode hint : sends a hint message to the specified field agent
  * the hint message must be less than or equal to 140 characters
  * if '*' is substituted for the agent hex code, the hint is sent to every friendly field agent


## Functional Summary
The guide agent is responsible for guiding his/her team's field agents while they play the game. The agent receives information about all code drops, including location, if they are still active, and who has captured them (if any).
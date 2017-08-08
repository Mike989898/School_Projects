# Opal Team - Mission Incomputable
## Chris Bertasi, Michael Kilgore, Andrew Laub
## May 2016

##A Bit About The Project
This project is meant to be a spy simulation, where each team has a maximum of four spys attempting to capture beacon points placed around campus. There are three roles in this game: The field agent (spy carrying a pebble smart watch), the game server (hosted by a computer), and a guide agent (also hosted by a computer). 
	
Field agents on each team are sent update messages to their pebble smart watches about the status the game by the game server, and forwarded 'hint' messages from the guide agent by the game server. If a field agent discovers the location of an enemy field agent, they can activate a capture radius by pressing a button on their smart watch. Every enemy field agent within that radius will be sent a capture code, which they must share with enemy field agents attempting to capture them. Only the field agent who initiates the capture will be able to enter capture codes into their watch for an alloted time period. Once a field agent is captured, they are out of the game. Field agents can also enter in capture codes for beacons that they discover on campus. Capturing all the beacons ends the game.

Guide agents are another type of client that connects to the game server, however they serve a different purpose. There is one guide agent per team, and guide agents are responsible for spending hint messages to each of the individual field agents on the team. Guide agents are privy to information about the coordinates of each of their team's field agents as well as the becaons on campus. With this information, they can lead their agents towards becaons in hopes that they will be able to capture them.

The game server takes care of all the functions related to passing information from guide agents to field agents and keeping track of the status of the game. Speficially, the game server manages traffic from guide agent and field agent connections, keeps track of all capture beacons, keeps track of all captured agents (and prevents them from rejoining a game in progress), and periodically sends update messages to field agents and guide agents about the status of the game.

## Build Instructions
* With a version of the make tool on your favorite shell, just type in the 'make' command to build each of the directories in the project.
* Note: In order to have the make tool build properly, you will need a C compiler and you will need to have the Pebble developer tool suite installed. You can install the Pebble SDK [here](https://developer.pebble.com/sdk/install/).

## Directory Summary
```
opal
├── Makefile
├── README.md
├── commonz
│   ├── Makefile
│   ├── file.c
│   ├── file.h
│   ├── index.c
│   ├── index.h
│   ├── message.c
│   ├── message.h
│   ├── pebble_strok.c
│   ├── pebble_strok.h
│   ├── socket.c
│   ├── socket.h
│   ├── web.c
│   ├── web.h
│   ├── word.c
│   └── word.h
├── docs
│   ├── DESIGN.md
│   ├── Field_Agent_GUI.jpg
│   ├── IMPLEMENTATION.md
│   ├── MI-network.png
│   ├── TESTING.md
│   ├── flow.dia
│   └── flow.png
├── field_agent
│   ├── BT-RX:
│   ├── Makefile
│   ├── README.md
│   ├── appinfo.json
│   ├── resources
│   │   ├── TerminusTTF.ttf
│   │   ├── images
│   │   │   └── Spy.png
│   │   └── perfect-dos-vga.ttf
│   ├── src
│   │   ├── Makefile
│   │   ├── commonz
│   │   │   ├── message.c
│   │   │   ├── message.h
│   │   │   ├── pebble_strok.c
│   │   │   └── pebble_strok.h
│   │   ├── field_agent.c
│   │   ├── js
│   │   │   └── app.js
│   │   ├── mission.h
│   │   └── windows
│   │       ├── game_message_window2.c
│   │       ├── game_message_window2.h
│   │       ├── pin_window.c
│   │       ├── pin_window.h
│   │       ├── pop_up_message_window.c
│   │       ├── pop_up_message_window.h
│   │       ├── selection_layer.c
│   │       ├── selection_layer.h
│   │       ├── team_agent_name_window.c
│   │       └── team_agent_name_window.h
│   └── wscript
├── game_server
│   ├── Makefile
│   ├── README.md
│   ├── cd.dat
│   ├── game_server.c
│   ├── map
│   │   ├── Makefile
│   │   ├── cdLoc.png
│   │   ├── faLoc.png
│   │   ├── map.c
│   │   ├── map.h
│   │   └── map.png
│   └── map2
│       ├── LICENSE
│       ├── Makefile
│       ├── README.md
│       ├── a.png
│       ├── animate
│       ├── boot
│       ├── ganimate.c
│       ├── gbootstrap.c
│       ├── hanlon.png
│       └── trump.png
├── guide_agent
│   ├── Makefile
│   ├── README.md
│   └── guideagent.c
├── lib
│   ├── Makefile
│   ├── bag
│   │   ├── Makefile
│   │   ├── README
│   │   ├── TESTING
│   │   ├── bag.c
│   │   └── bag.h
│   ├── counters
│   │   ├── Makefile
│   │   ├── README
│   │   ├── TESTING
│   │   ├── counters.c
│   │   └── counters.h
│   ├── hashtable
│   │   ├── Makefile
│   │   ├── README
│   │   ├── TESTING
│   │   ├── hashtable.c
│   │   └── hashtable.h
│   ├── list
│   │   ├── Makefile
│   │   ├── README
│   │   ├── TESTING
│   │   ├── list.c
│   │   └── list.h
│   └── memory
│       ├── Makefile
│       ├── memory.c
│       └── memory.h
└── proxy
    ├── package.json
    └── proxy
```


### Game Server
* Hosts the actual game
* Relays information about the current state of the game to all connected agents
* Receives information from field agents when they have captured a code drop or an enemy field agent
* Forwards hints from the guide agent to specified allied field agents
* Alerts all agents when the game has ended

### Guide Agent
* Receives game information from the game server
* Guides friendly field agents towards code drops
  * Can send 'hint' messages to specified teammates

### Field Agent
* Hunts for the code drops
* Can 'capture' active code drops
* Can 'capture' enemy field agents, removing them from the game
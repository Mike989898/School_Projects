# Design Document for Pebble Project
#### _By Chris Bertasi, Michael Kilgore, and Andrew Laub_

## Requirements Specifcation
Please the [CS50 Requirements Specification](http://www.cs.dartmouth.edu/~cs50/Labs/Project/mission.html) page for all of the 
requirements of each of the components.

----

## Design Specification
### User Interface

#### Field Agent Interface
* The Field Agent interface will be a simple text-display interface with buttons on the pebble mapped to specifc actions:
  * One of the pebble buttons will be mapped to a 'capture' action, enabling the field agents to capture other agents and beacons. Pushing the
  capture button will bring the field agent to a different screen where they will be allowed to enter the 4-digit hex capture code.
  * One of the pebble buttons will be mapped to an 'active' action, allowing the field agent to become active and start capturing other agents and beacons.
  Pushing the active button will change the color of the interface to indicate to the agent that they are now active(for the new color scheme we are thinking of a black-green, matrix-style theme). 
  Pushing the button again will allow the agent to become deactive(returning the color of the display to its default state).
  * The main screen will be an area where the player can see mesasges from their field guide as well as messages from the game server informing them how much time is left in the game. Messages will appear in chronological, and messages from the game server will appear in <span style="color:red">red</span>. The game server will also
  send 4-digit hex codes to the field agent, informing them that they are a target of a capture action by another agent. We are considering implementing
  the vibration feature of the pebble time to notify the agent that they are the target of a capture.
#### Guide Interface
* The Guide interface will implement XQuartz to have a graphical overlay, which will include a map of the campus with colored markers for all friendly field agents and all code drop locations.
* The Guide interface will have a text-input field where the guide can send messages to their field agents(of no more than 140 characters).

#### Server Interface
* The Server interface will also implement XQuartz to have a graphical overlay, including a map of the campus, with different colored markers for all field agents of all teams and the locations of every code drop.

----

### Inputs and Outputs
* The Field Agent and guide communication will be indirectly through the proxy server.
* The Guide agent will also communicate through the server (indirectly through the proxy server) to the field agent.
* The server will periodically send messages to the guide agents when a code drop has been captured. The guide agents will then communicate back to the field agents that a code drop is no longer active.
* The field agent will periodically communicate to the game server to alert it if they have captured either a code drop, or if it has captured an enemy field agent.
* The field agent will update its location to the guide agent about every 15 seconds
  * This will change to once a minute if the pebble accelerometer determines that the field agent is not moving (extra credit).
* Both the Game Server and Guide agents will produce log files of all received and transmitted information.

----

### Functional Decomposition into Modules
* We plan to implement three modules: the game server module, the guide agent module, and the field agent module.

#### Game Server Pseudo Code
    Read list of code drops from file and store them in a list.
    Create a DGRAM socket and bind it to the given port number; exit on any failure.
    Pick a random game number between 1 and 65535 for the new game identifier.
    Listen for datagrams.
    parse datagram and figure out what kind of information packet it is.
      if the packet contains registration information, add the field agent or guide to 
      the hashtable ignoring any bad requests as described in the requirments section.
      else if the packet contains information about game status updates(ie beacon or player captured) update the hashtable.
#### Guide Pseudo Code
    Register to a particular game.
    Request information about how many field agents are registered to this specific guide instance.
    Request information about the location of the code drops to display on the map.
    receive updates in the form of <LatLng> and update the map position of each of the registered field.
    Send messages to the server that will be distributed to each of the field agents.
#### Field Agent Pseudo Code
    Register to a particular game.
    send out <LatLng> updates.
    receive messages from gameserver and update textview.
    if button input = active, change to active status
    else if button input = capture change viewing window to the capture window.

Note that the above are **_brief_** pseudo codes of the three components used in the lab. For a more detailed
decription of each module, see the page linked in the requirements section.

----

### Major Data Structures
* We plan on using our generic data structures coded in Lab 3 for use in this lab.
* We anticipate using a hashtable keyed by guide(mapping to a list of field agents), which will be used in the game server module.
* We anticipate representing the code drops as a list of code-drop structs (we plan on deleting code drops from the list once they've been deactivated).
* We anticipate creating structs to represent field agents that will contain information about the status of the agent(captured, active, ect...).

----

### Testing Plan
We plan to have a bash script for automated testing of each of the three components.
* Generate a file of random coordinate to initialize code drops for the game server and field guide and ensure that they appear in the correct locations on the map
* Testing that a client can reconnect to a game after dropping the wifi
* Testing that the client cannot reconnect to a game that's already finished.
* Testing that clients can successfully "capture" an enemy agent, and that the captured agent will be removed from the game
* Testing that clients can successfully capture code drops, and that the code drops are then deactivated and can not be re-captured
* Testing that the game successfully ends when the timer is up.
* Using Professor Kotz's memory package to check that the number of mallocs and frees match.

----

### Planned Extra Credit

<html>
<head>
<style>
table {
    width:100%;
}
table, th, td {
    border: 1px solid black;
    border-collapse: collapse;
}
th, td {
    padding: 5px;
    text-align: left;
}
table#t01 tr:nth-child(even) {
    background-color: #eee;
}
table#t01 tr:nth-child(odd) {
   background-color:#fff;
}
table#t01 th {
    background-color: Grey;
}

</style>
</head>
<body>
<table id="t01">
  <tr>
    <th>Extension</th>
    <th>Points</th>
  </tr>
  <tr>
    <td>Game server graphical summary.</td>
    <td>10</td>
  </tr>
  <tr>
    <td>Guide agent graphical summary.</td>
    <td>5</td>
  </tr>
  <tr>
    <td>Using the accelerometer to determine if a field agent is standing still.</td>
    <td>5</td>
  </tr>
  <tr>
    <td>Using the compass to determine which way a player is facing.</td>
    <td>5</td>
  </tr>
  <tr>
    <td>Using the proper equations for latitude and longitude to determine distance.</td>
    <td>3</td>
  </tr>
  <tr>
    <td>Implementing a game server timer for a maximum game length.</td>
    <td>2</td>
  </tr>
  <tr>
    <td>Considering enhancing the field agent pebble UI to provide a more interesting display.</td>
    <td>10</td>
  </tr>
</table>

</body>
</html>


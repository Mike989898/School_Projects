# Game server

This is a game server to host pebble field agents and guide agents.

Launch as such: `./gameserver [-v] port codeDrops`

Where -v is a flag that enables verbose logging, `port` is the port the server runs on, and `codeDrops` is a file containing code drops information, formatting as per the design spec.

The server logs out put in the `project/log/gameserver.log` file.

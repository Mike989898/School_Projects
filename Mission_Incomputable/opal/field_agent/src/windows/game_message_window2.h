#ifndef __GAME_MESSAGE_WINDOW2_H
#define __GAME_MESSAGE_WINDOW2_H

#include <pebble.h>

//create the game message window
void game_message_window_create(char *game_msg);

//push the window
void game_message_window_push();

void game_message_window_destroy();

#endif // __GAME_MESSAGE_WINDOW2_H
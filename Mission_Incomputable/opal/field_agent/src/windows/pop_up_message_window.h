#ifndef __POP_UP_MESSAGE_WINDOW_H
#define __POP_UP_MESSAGE_WINDOW_H

void pop_up_window_create(char *pop_up_message, GColor color1, GColor color2, int dur, bool is_timed);
void pop_up_window_push();
void pop_up_window_destroy();
void pop_up_window_pop();


#endif // __POP_UP_MESSAGE_WINDOW_H
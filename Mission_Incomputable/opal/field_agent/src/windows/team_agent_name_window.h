#ifndef __TEAM_AGENT_NAME_WINDOW_H
#define __TEAM_AGENT_NAME_WINDOW_H

typedef void (*NameSelectComplete)(int index, void *context);

typedef struct NameSelectCallbacks {
    NameSelectComplete agent_name_selected;
    NameSelectComplete team_name_selected;
} NameSelectCallbacks;

void create_team_agent_name_window();

void push_team_agent_name_window();

void destroy_team_agent_name_window();

#endif // __TEAM_AGENT_NAME_WINDOW_H
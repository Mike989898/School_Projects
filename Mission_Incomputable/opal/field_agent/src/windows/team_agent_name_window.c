#include "pebble.h"
#include "team_agent_name_window.h"

#define NUM_MENU_SECTIONS 2
#define NUM_FIRST_MENU_ITEMS 4
#define NUM_SECOND_MENU_ITEMS 4

static Window *s_main_window;
static SimpleMenuLayer *s_simple_menu_layer;
static SimpleMenuSection s_menu_sections[NUM_MENU_SECTIONS];
static SimpleMenuItem s_first_menu_items[NUM_FIRST_MENU_ITEMS];
static SimpleMenuItem s_second_menu_items[NUM_SECOND_MENU_ITEMS];
static NameSelectCallbacks *myNamesCallbacks;

void team_agent_name_window_load(Window *window) {

  s_first_menu_items[0] = (SimpleMenuItem) {
    .title = "Opal",
    .callback = myNamesCallbacks->team_name_selected,
  };
  s_first_menu_items[1] = (SimpleMenuItem) {
    .title = "Diamond",
    .callback = myNamesCallbacks->team_name_selected,
  };
  s_first_menu_items[2] = (SimpleMenuItem) {
    .title = "Ruby",
    .callback = myNamesCallbacks->team_name_selected,
  };

  s_first_menu_items[3] = (SimpleMenuItem) {
    .title = "Jade",
    .callback = myNamesCallbacks->team_name_selected,
  };

  s_second_menu_items[0] = (SimpleMenuItem) {
    .title = "Alpha",
    .callback = myNamesCallbacks->agent_name_selected,
  };

  s_second_menu_items[1] = (SimpleMenuItem) {
    .title = "Bravo",
    .callback = myNamesCallbacks->agent_name_selected,
  };

  s_second_menu_items[2] = (SimpleMenuItem) {
    .title = "Charlie",
    .callback = myNamesCallbacks->agent_name_selected,
  };

  s_second_menu_items[3] = (SimpleMenuItem) {
    .title = "Delta",
    .callback = myNamesCallbacks->agent_name_selected,
  };

  s_menu_sections[0] = (SimpleMenuSection) {
    .title = "Select Team Name",
    .num_items = NUM_FIRST_MENU_ITEMS,
    .items = s_first_menu_items,
  };
  s_menu_sections[1] = (SimpleMenuSection) {
    .title = "Select Agent Name",
    .num_items = NUM_SECOND_MENU_ITEMS,
    .items = s_second_menu_items,
  };

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  s_simple_menu_layer = simple_menu_layer_create(bounds, window, s_menu_sections, NUM_MENU_SECTIONS, NULL);

  layer_add_child(window_layer, simple_menu_layer_get_layer(s_simple_menu_layer));
}

void team_agent_name_window_unload(Window *window) {
  simple_menu_layer_destroy(s_simple_menu_layer);
}

void create_team_agent_name_window(NameSelectCallbacks *namesCallbacks) {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = team_agent_name_window_load,
    .unload = team_agent_name_window_unload,
  });
  myNamesCallbacks = namesCallbacks;
}

void push_team_agent_name_window(){
  window_stack_push(s_main_window, true);
}

void destroy_team_agent_name_window() {
  window_destroy(s_main_window);
}

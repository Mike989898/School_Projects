#include "pebble.h"

static Window *s_main_window;

// This is a scroll layer
static ScrollLayer *s_scroll_layer;

// We also use a text layer to scroll in the scroll layer
static TextLayer *s_text_layer;

// The scroll layer can other things in it such as an invert layer
//static InverterLayer *s_inverter_layer;

// Lorum ipsum to have something to scroll
static char *s_scroll_text = "";

// Setup the scroll layer on window load
// We do this here in order to be able to get the max used text size
static void game_message_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  GRect max_text_bounds = GRect(0, 0, bounds.size.w, 2000);

  // Initialize the scroll layer
  s_scroll_layer = scroll_layer_create(bounds);
  scroll_layer_set_shadow_hidden(s_scroll_layer, true);
  // This binds the scroll layer to the window so that up and down map to scrolling
  // You may use scroll_layer_set_callbacks to add or override interactivity
  scroll_layer_set_click_config_onto_window(s_scroll_layer, window);

  // Initialize the text layer
  s_text_layer = text_layer_create(max_text_bounds);
  text_layer_set_text(s_text_layer, s_scroll_text);
  text_layer_set_background_color(s_text_layer, GColorBlack);
  text_layer_set_text_color(s_text_layer, GColorGreen);
  // Change the font to a nice readable one
  // This is system font; you can inspect pebble_fonts.h for all system fonts
  // or you can take a look at feature_custom_font to add your own font
  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));

  // Trim text layer and scroll content to fit text box
  GSize max_size = text_layer_get_content_size(s_text_layer);
  text_layer_set_size(s_text_layer, max_size);
  scroll_layer_set_content_size(s_scroll_layer, GSize(bounds.size.w, max_size.h + 4));

  // Add the layers for display
  scroll_layer_add_child(s_scroll_layer, text_layer_get_layer(s_text_layer));

  // The inverter layer will highlight some text
  //s_inverter_layer = inverter_layer_create(GRect(0, 28, bounds.size.w, 28));
  //scroll_layer_add_child(s_scroll_layer, inverter_layer_get_layer(s_inverter_layer));

  layer_add_child(window_layer, scroll_layer_get_layer(s_scroll_layer));
}

static void game_message_window_unload(Window *window) {
  //inverter_layer_destroy(s_inverter_layer);
  text_layer_destroy(s_text_layer);
  scroll_layer_destroy(s_scroll_layer);
}

void game_message_window_create(char *game_msg) {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = game_message_window_load,
    .unload = game_message_window_unload,
  });
  s_scroll_text = game_msg;
  window_stack_push(s_main_window, true);
}

void game_message_window_push(){
  window_stack_push(s_main_window, true);
}

void game_message_window_destroy() {
  window_destroy(s_main_window);
}

#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_message_layer;
static GFont s_time_font;
static GColor backgroundColor;
static GColor textColor;
static int duration_in_ms;
static char *message_to_display = "";
static AppTimer *my_timer;
static bool there_is_timer = false;

void timer_callback(void *data) {
	window_stack_pop(true);
	window_destroy(s_main_window);
}

void pop_up_window_load(Window *window) {
	// Get information about the Window
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	// Create the TextLayer with specific bounds
	s_message_layer = text_layer_create(
		GRect(0, 0, bounds.size.w, bounds.size.h));

	text_layer_set_background_color(s_message_layer, backgroundColor);
	text_layer_set_text_color(s_message_layer, textColor);
	text_layer_set_font(s_message_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	text_layer_set_text(s_message_layer, message_to_display);

	layer_add_child(window_layer, text_layer_get_layer(s_message_layer));
	if(there_is_timer){
		//second timer
		my_timer = app_timer_register(duration_in_ms, (AppTimerCallback) timer_callback, NULL);
	}
}

void pop_up_window_unload(Window *window) {
	// Destroy TextLayer
	text_layer_destroy(s_message_layer);
	if(there_is_timer){
		//cancel the timer
		app_timer_cancel(my_timer);
	}
}

void pop_up_window_create(char *pop_up_message, GColor color1, GColor color2, int dur, bool is_timed) {
	// Create main Window element and assign to pointer
	s_main_window = window_create();

	// Set handlers to manage the elements inside the Window
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = pop_up_window_load,
		.unload = pop_up_window_unload
	});
	backgroundColor = color1;
	textColor = color2;
	duration_in_ms = dur;
	message_to_display = pop_up_message;
	there_is_timer = is_timed;
}

void pop_up_window_push(){
	window_stack_push(s_main_window, true);
}

void pop_up_window_pop(){
	window_stack_pop(true);
	window_destroy(s_main_window);
}

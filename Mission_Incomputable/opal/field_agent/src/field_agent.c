#include <pebble.h>

// Local includes
#include "mission.h"
#include "windows/game_message_window2.h"
#include "windows/pin_window.h"
#include "windows/team_agent_name_window.h"
#include "windows/pop_up_message_window.h"
#include "./commonz/message.h"

#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ITEMS 5

//structure for holding key-value pairs for a message retry
typedef struct message_retry {
  int key;
  char *value;
} message_retry_t;


/*************************Global Variables************************************/

//Variables for UI elements
static Window *s_main_window;
static PinWindow *pin_window;
static SimpleMenuLayer *s_simple_menu_layer;
static SimpleMenuSection s_menu_sections[NUM_MENU_SECTIONS];
static SimpleMenuItem s_first_menu_items[NUM_MENU_ITEMS];
static NameSelectCallbacks *nameCallbacks;
static AppTimer *s_timeout_timer;

//Booleans to indicate status of agent
static bool agent_active_status = false;
static bool agent_has_name = false;
static bool agent_has_team = false;
static bool have_pebble_id = false;
static bool capture_timer_is_up = true;
static bool first_message = true;
static bool have_lat_lng = false;
static bool can_send_messages = false; // variable for determining whether the js environment is ready or not

//Variables to hold agent and games stats
static char* agent_name = "";
static char* team_name = "";
static int16_t average_old_x = 0;
static int16_t average_old_y = 0;
static int16_t current_headding = 0;
static char *latlng = "none";
static char *pebbleId = "1234";
static int seconds_past = 1;
static int gameId = 0;
static char *game_messages = "None Yet.";
static int numRemainingCodeDrops = 0;
static int numFriendlyOperatives = 0;
static int numFoeOperatives = 0;

//inactive/hint vibrate pattern
static const VibePattern CUSTOM_PATTERN = {
  .durations = (uint32_t[]) {50, 100, 100, 50},
  .num_segments = 4
};

//active vibrate pattern
static const VibePattern CUSTOM_PATTERN_2 = {
  .durations = (uint32_t[]) {50, 100},
  .num_segments = 2
};

//capture vibrate pattern
static const VibePattern CUSTOM_PATTERN_3 = {
  .durations = (uint32_t[]) {50, 500},
  .num_segments = 2
};

/*Prototypes*/
static void send_message(int key, char *value, bool val_malloced); 
static char *build_location_message();
static char *build_neutralize_message(char* code_ID);
static char *build_capture_message(char* code_ID);
static char *convert_pin_to_string(PIN pin);
static void handle_message(msg_t *svrmsg);

/****************************Callback Functions*********************************/

void timer_captured_callback(void *data) {
  pop_up_window_create("You were captured by other agent!", GColorBlack, GColorYellow, 1500, true);
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  // Successful message, the timeout is not needed anymore for this message
  app_timer_cancel(s_timeout_timer);
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}


//callback for pin enter window for neutralizing code drop
static void pin_complete_neutralize_callback(PIN pin, void *context) {
  //printf("Pin was %d %d", pin.digits[0], pin.digits[1]);
  char *comb = convert_pin_to_string(pin);
  //printf("The combination was: %s", comb);
  send_message(AppKeySendMsg, build_neutralize_message(comb) , true);
  pin_window_pop((PinWindow*)context, true);
}

//callback for pin enter window for neutralizing code drop
static void pin_complete_capture_callback(PIN pin, void *context) {
  //printf("Pin was %d %d", pin.digits[0], pin.digits[1]);
  char *comb = convert_pin_to_string(pin);
  send_message(AppKeySendMsg, build_capture_message(comb) , true);
  pin_window_pop((PinWindow*)context, true);
}

//callback for selecting team name
static void team_name_select_callback(int index, void *ctx) {
  switch(index){
    case 0:
    team_name = "Opal";
    break;
    case 1:
    team_name = "Diamond";
    break;
    case 2:
    team_name = "Ruby";
    break;
    case 3:
    team_name = "Jade";
    break;
  }
  agent_has_team = true;
  pop_up_window_create("Team name selected.", GColorBlue, GColorOrange, 1000, true);
  pop_up_window_push();
}

//callback for selecting agent name
static void agent_name_select_callback(int index, void *ctx) {
    switch(index){
    case 0:
    agent_name = "Alpha";
    break;
    case 1:
    agent_name = "Bravo";
    break;
    case 2:
    agent_name = "Charlie";
    break;
    case 3:
    agent_name = "Delta";
    break;
  }
  agent_has_name = true;
  pop_up_window_create("Agent name selected.", GColorGreen, GColorRed, 1000, true);
  pop_up_window_push();
}

void capture_timer_callback(void *data) {
  capture_timer_is_up = true;
}

static void menu_select_callback(int index, void *ctx) {
  if((!agent_has_name || !agent_has_team) && (index != 0)){
    pop_up_window_create("Must select agent name and team name before using other agent actions.", GColorBlack, GColorRed, 2000 , true);
    pop_up_window_push();
    return;
  }
  switch(index){
    case 0:
    create_team_agent_name_window(nameCallbacks);
    push_team_agent_name_window();
    break;
    case 1:
    if(!agent_active_status){
      vibes_enqueue_custom_pattern(CUSTOM_PATTERN_2);
      agent_active_status = true;
    } else{
      vibes_enqueue_custom_pattern(CUSTOM_PATTERN);
      agent_active_status = false;
    }
    break;
    case 2:{
      pin_window = pin_window_create((PinWindowCallbacks) {
        .pin_complete = pin_complete_neutralize_callback
      });
      pin_window_set_highlight_color(pin_window, GColorBlue);
      pin_window_push(pin_window, true);
    }
    break;
    case 3:
    if(capture_timer_is_up){
      pop_up_window_create("4-digit hex code dispersed to all agents within 10 meters of you. Push Capture again to enter code.", GColorPurple, GColorYellow, 3000, true);
      pop_up_window_push();
      capture_timer_is_up = false;
      send_message(AppKeySendMsg, build_capture_message("0"), true);
      //wait 60 seconds before next capture message
      app_timer_register(60000, (AppTimerCallback) capture_timer_callback, NULL);
    }
    else{
      pin_window = pin_window_create((PinWindowCallbacks) {
        .pin_complete = pin_complete_capture_callback
      });
      pin_window_set_highlight_color(pin_window, GColorBlue);
      pin_window_push(pin_window, true);
    }
    break;
    case 4:
    game_message_window_create(game_messages);
    game_message_window_push();
    break;
  }
}

/****************************** Handler Functions *******************************/
static void handle_message(msg_t *svrmsg){
  if((strcmp(svrmsg->opCode, "GAME_STATUS")) == 0){
    if((svrmsg->gameId != gameId) && (gameId != 0)){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Received invalid GA_STATUS game id. Game ID was: %d and should be: %d", svrmsg->gameId, gameId);
      return;
    }else{
      numRemainingCodeDrops = svrmsg->numRemainingCodeDrops;
      numFriendlyOperatives = svrmsg->numFriendlyOperatives;
      numFoeOperatives = svrmsg->numFoeOperatives;
      gameId = svrmsg->gameId;
    }
  }
  else if((strcmp(svrmsg->opCode, "GS_CAPTURE_ID")) == 0){
    if((svrmsg->gameId != gameId)){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Received invalid GS_CAPTURE_ID game id. Game ID was: %d and should be: %d", svrmsg->gameId, gameId);
      return;
    }
    char *capture_code_string = (char*) malloc(30*sizeof(char));
    strcpy(capture_code_string, "Your capture code is: ");
    strcat(capture_code_string, svrmsg->captureId);
    vibes_enqueue_custom_pattern(CUSTOM_PATTERN_3);
    pop_up_window_create(capture_code_string, GColorBlack, GColorGreen, 60000, true);
  }
  else if((strcmp(svrmsg->opCode, "GS_RESPONSE")) == 0){
    if((svrmsg->gameId != gameId)){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Received invalid GS_CAPTURE_ID game id. Game ID was: %d and should be: %d", svrmsg->gameId, gameId);
      return;
    }
    if(strcmp(svrmsg->respCode, "MI_ERROR_INVALID_OPCODE")){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Pebble sent invalid OP_CODE");
    }
    if(strcmp(svrmsg->respCode, "MI_ERROR_INVALID_GAME_ID")){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Pebble sent invalid GAME_ID");
    }
    if(strcmp(svrmsg->respCode, "MI_ERROR_INVALID_TEAMNAME")){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Pebble sent invalid TEAM_NAME");
    }
    if(strcmp(svrmsg->respCode, "MI_ERROR_INVALID_ID")){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Pebble sent invalid PEBBLE_ID and GUIDE_ID sent by pebble");
    }
    if(strcmp(svrmsg->respCode, "MI_NEUTRALIZED")){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Agent(ME) neutralized code drop successfully");
      pop_up_window_create("Code drop sucessfully neutralized!", GColorBlack, GColorYellow, 1500, true);
    }
    if(strcmp(svrmsg->respCode, "MI_CAPTURE_SUCCESS")){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Agent(ME) captured other agent successfully");
      pop_up_window_create("Agent successfully Captured!", GColorBlack, GColorYellow, 1500, true);
    }
    if(strcmp(svrmsg->respCode, "MI_CAPTURED")){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Agent(ME) captured by other agent");
      pop_up_window_pop();
      app_timer_register(1500, (AppTimerCallback) timer_captured_callback, NULL);
    }
  }
  else if((strcmp(svrmsg->opCode, "GA_HINT")) == 0){
    if(svrmsg->gameId != gameId){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Received invalid GA_HINT game id. Game ID was: %d and should be: %d", svrmsg->gameId, gameId);
      free(svrmsg->teamName);
      free(svrmsg->playerName);
      free(svrmsg->pebbleId);
      free(svrmsg->message);
      return;
    }
    if((strcmp(svrmsg->teamName, team_name)) != 0){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Received invalid GA_HINT team name. Team Name was: %s and should be: %s", svrmsg->teamName, team_name);
      free(svrmsg->teamName);
      free(svrmsg->playerName);
      free(svrmsg->pebbleId);
      free(svrmsg->message);
      return;
    }
    if((strcmp(svrmsg->pebbleId, pebbleId)) != 0){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Received invalid GA_HINT pebble id. pebbleId was: %s and should be: %s", svrmsg->pebbleId, pebbleId);
      free(svrmsg->teamName);
      free(svrmsg->playerName);
      free(svrmsg->pebbleId);
      free(svrmsg->message);
      return;
    }
    vibes_enqueue_custom_pattern(CUSTOM_PATTERN);
    pop_up_window_create(svrmsg->message, GColorBlack, GColorGreen, 30000, false);
    pop_up_window_push();
  }
  else if((strcmp(svrmsg->opCode, "GAME_OVER")) == 0){
  }
}


//in and outbox methods
static void inbox_received_handler(DictionaryIterator *iterator, void *context) {
  // Check: Is this a PebbleKit JS ready message?! If so, then it is safe to send messages!
  Tuple *ready_tuple = dict_find(iterator, AppKeyJSReady);
  if(ready_tuple) {
    // Log the value sent as part of the received message.
    char *ready_str = ready_tuple->value->cstring;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Got AppKeyJSReady: %s", ready_str);
    can_send_messages = true;
  }
  //see if a message from the game server was received
  Tuple *recv_message_tuple = dict_find(iterator,AppKeyRecvMsg);
  if(recv_message_tuple){
    char *recv_message = recv_message_tuple->value->cstring;
    msg_t *server_message = message_parse(recv_message);
    if(server_message == NULL){
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Message parse was Null!!!");
      return;
    }
    handle_message(server_message);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Got AppKeyRecvMsg: %s", recv_message);
  }
  //check for request gps update
  Tuple *recv_gps_update_tuple = dict_find(iterator,AppKeyLocation);
  if(recv_gps_update_tuple){
    char *recv_gps_update = recv_gps_update_tuple->value->cstring;
    if(strcmp(latlng,"none") == 0){
      latlng = (char*) malloc(300*sizeof(char));
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Got here");
    }
    strcpy(latlng,recv_gps_update);
    have_lat_lng = true; 
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Got AppKeyLocation: %s", recv_gps_update);
  }
  //check for id request update
  Tuple *recv_id_update_tuple = dict_find(iterator, AppKeyPebbleId);
  if(recv_id_update_tuple){
    char *recv_id_update = recv_id_update_tuple->value->cstring;
    pebbleId = (char*) malloc(300*sizeof(char));
    strcpy(pebbleId,recv_id_update);
    have_pebble_id = true;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "AppKeyPebbleId: %s", recv_id_update);
  }

  Tuple *error_tuple = dict_find(iterator,AppKeySendError);
  if(error_tuple){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Error in receiving message!");
  }
}

static void timout_timer_handler(void *data) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Failed. Retrying...");
  message_retry_t *message = (message_retry_t *) data;
  // Retry the message
  int retry_key = message->key;
  char *retry_value = message->value;
  free(message);//free the message_retry struct
  send_message(retry_key, retry_value, false);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  if(have_pebble_id){
    if(have_lat_lng){
      if((seconds_past % 5) == 0 && (seconds_past !=1) && ((seconds_past % 15) != 0)){
        send_message(AppKeyLocation, "filler", false);
      }
      if(((seconds_past % 15) == 0)){
        if((seconds_past == 60)){
          seconds_past = 0;
          send_message(AppKeySendMsg,build_location_message("1") , true);
          //send_message(AppKeySendMsg, build_location_message("1"), true);
        }
        else{
          if(first_message){
            send_message(AppKeySendMsg,build_location_message("1") , true);
            first_message = false;
          }
          else{
            send_message(AppKeySendMsg,build_location_message("0") , true);
          }
          //send_message(AppKeySendMsg, build_location_message("0"), true);
        }
      }
    }
  }
  if(!have_pebble_id && (seconds_past>2)){
    send_message(AppKeyPebbleId, "filler", false);
  }
  if((!have_lat_lng) && (seconds_past>3)){
    send_message(AppKeyLocation, "filler", false);
  }
  seconds_past++;
}

static void accel_data_handler(AccelData *data, uint32_t num_samples) {
  int16_t average_x = 0;
  int16_t average_y = 0;

  int i;
  for(i=0; i<100; i++){
    average_x += data[i].x;
    average_y += data[i].y;
  }

  average_x = average_x/100;
  average_y = average_y/100;

  int16_t diff_x = abs(average_x - average_old_x);
  int16_t diff_y = abs(average_y - average_old_y);

  average_old_x = average_x;
  average_old_y = average_y; 

  if(abs(diff_x) > 40 || abs(diff_y) > 40){
    //update moving status here
  }
}

static void compass_heading_handler(CompassHeadingData heading_data) {
  current_headding = TRIGANGLE_TO_DEG(heading_data.magnetic_heading);
}

/******************Helper functions************************************/

static void send_message(int key, char *value, bool val_malloced) {
    //check the environment is ready
  if(can_send_messages) {
    //check the agent is active
    if(agent_active_status){
      // Declare the dictionary's iterator
      DictionaryIterator *iter;

      // Prepare the outbox buffer for this message
      AppMessageResult result = app_message_outbox_begin(&iter);

      if(result == APP_MSG_OK) {
        // Construct & send the message.
        dict_write_cstring(iter, key, value);
        result = app_message_outbox_send();
        if(result != APP_MSG_OK) {
          //create a retry message
          message_retry_t *message = malloc(sizeof(message_retry_t));
          message->key = key;
          message->value = value;

          // Schedule the timeout timer
          const int interval_ms = 1000;
          s_timeout_timer = app_timer_register(interval_ms, timout_timer_handler, message);
          APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
        }
        else{
          if(val_malloced){
        free(value); //free the message if we built the string
      }
    }
  } else {
      // The outbox cannot be used right now
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
      }
    }
  }
}

static char *build_location_message(char *status_update){
  char *location_message = (char*) malloc(800*sizeof(char)); // message should be no longer than 300 characters
  //copy the first string in to make sure there's a null terminator for strcat
  APP_LOG(APP_LOG_LEVEL_DEBUG,"PebbleId: %s", pebbleId);
  char *strn = "FA_LOCATION";
  strcpy(location_message, strn);
  strcat(location_message, "|");

  //add game id
  char *game_id_string = (char*) malloc(8*sizeof(char));
  snprintf(game_id_string, 8, "%d", gameId);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "game_id %s", game_id_string);
  strcat(location_message, game_id_string);
  strcat(location_message, "|");

  //add pebble id
  strcat(location_message, pebbleId);
  strcat(location_message, "|");

  //add Team name
  strcat(location_message,team_name);
  strcat(location_message, "|");

    //add player name
  strcat(location_message, agent_name);
  strcat(location_message, "|");

  //add latlng
  strcat(location_message, latlng);
  strcat(location_message, "|");

  //adding status request
  strcat(location_message, status_update);

  return location_message;
}

static char *build_neutralize_message(char* code_ID){
  char *neutralize_message = (char*) malloc(300*sizeof(char)); // message should be no longer than 300 characters
  char *strn ="FA_NEUTRALIZE";
  strcpy(neutralize_message, strn);
  strcat(neutralize_message, "|");

  //add game id
  char *game_id_string = (char*) malloc(8*sizeof(char));
  snprintf(game_id_string, 8, "%d", gameId);
  strcat(neutralize_message, game_id_string);
  strcat(neutralize_message, "|");

  //add pebble id
  strcat(neutralize_message, pebbleId);
  strcat(neutralize_message, "|");

    //add Team name
  strcat(neutralize_message,team_name);
  strcat(neutralize_message, "|");

  //add player name
  strcat(neutralize_message, agent_name);
  strcat(neutralize_message, "|");

  //add latlng
  strcat(neutralize_message, latlng);
  strcat(neutralize_message, "|");

  //add Code ID
  strcat(neutralize_message, code_ID);
  //free the string combination that was built using malloc
  free(code_ID);
  return neutralize_message;
}

static char *build_capture_message(char* code_ID){
  char *capture_message = (char*) malloc(300*sizeof(char)); // message should be no longer than 300 characters
  char *strn = "FA_CAPTURE";
  strcpy(capture_message, strn);
  strcat(capture_message, "|");

  //add game id
  char *game_id_string = (char*) malloc(8*sizeof(char));
  snprintf(game_id_string, 8, "%d", gameId);
  strcat(capture_message, game_id_string);
  strcat(capture_message, "|");

  //add pebble id
  strcat(capture_message, pebbleId);
  strcat(capture_message, "|");

    //add Team name
  strcat(capture_message,team_name);
  strcat(capture_message, "|");

  //add player name
  strcat(capture_message, agent_name);
  strcat(capture_message, "|");

  //add Code ID
  strcat(capture_message, code_ID);
  if(strcmp(code_ID, "0") != 0){
    free(code_ID);
  }
  return capture_message;
}

static char* convert_pin_to_string(PIN pin){
  char *combination = (char*) malloc(10*sizeof(char));
  combination[0] = '\0';
  //four pin digits
  int i;
  for(i=0; i<4; i++){
    switch(pin.digits[i]){
      case 0:
        strcat(combination, "0");
        break;
      case 1:
        strcat(combination, "1");
        break;
      case 2:
        strcat(combination, "2");
        break;
      case 3:
        strcat(combination, "3");
        break;
      case 4:
        strcat(combination, "4");
        break;
      case 5:
        strcat(combination, "5");
        break;
      case 6:
        strcat(combination, "6");
        break;
      case 7:
        strcat(combination, "7");
        break;
      case 8:
        strcat(combination, "8");
        break;
      case 9:
        strcat(combination, "9");
        break;
      case 10:
        strcat(combination, "A");
        break;
      case 11:
        strcat(combination, "B");
        break;
      case 12:
        strcat(combination, "C");
        break;
      case 13:
        strcat(combination, "D");
        break;
      case 14:
        strcat(combination, "E");
        break;
      case 15:
        strcat(combination, "F");
        break;
    }
  }
  return combination;
}

static void main_window_load(Window *window) {

  s_first_menu_items[0] = (SimpleMenuItem) {
    .title = "Team/agent Name.",
    .subtitle = "Pick team and agent name.",
    .callback = menu_select_callback,
  };
  s_first_menu_items[1] = (SimpleMenuItem) {
    .title = "Become Active!",
    .subtitle = "Push again for inactive.",
    .callback = menu_select_callback,
  };
  s_first_menu_items[2] = (SimpleMenuItem) {
    .title = "Neutralize!",
    .callback = menu_select_callback,
  };
  s_first_menu_items[3] = (SimpleMenuItem) {
    .title = "Capture!",
    .callback = menu_select_callback,
  };
    s_first_menu_items[4] = (SimpleMenuItem) {
    .title = "Game Messages.",
    .callback = menu_select_callback,
  };
  s_menu_sections[0] = (SimpleMenuSection) {
    .num_items = NUM_MENU_ITEMS,
    .items = s_first_menu_items,
  };

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  s_simple_menu_layer = simple_menu_layer_create(bounds, window, s_menu_sections, NUM_MENU_SECTIONS, NULL);

  layer_add_child(window_layer, simple_menu_layer_get_layer(s_simple_menu_layer));
}

void main_window_unload(Window *window) {
  simple_menu_layer_destroy(s_simple_menu_layer);
}


static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
  //malloc space for the callbacks struct
  nameCallbacks = malloc(sizeof(NameSelectCallbacks));
  nameCallbacks->agent_name_selected = agent_name_select_callback;
  nameCallbacks->team_name_selected = team_name_select_callback;

// Register Bluetooth communication callbacks
app_message_register_inbox_received(inbox_received_handler);
app_message_register_inbox_dropped(inbox_dropped_callback);
app_message_register_outbox_failed(outbox_failed_callback);
app_message_register_outbox_sent(outbox_sent_callback);

// Open `AppMessage` - to always get the largest buffer available, follow this best practice:
app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

// Subscribe to batched data events from accelerometer
accel_service_set_sampling_rate(ACCEL_SAMPLING_10HZ);
accel_data_service_subscribe(100, accel_data_handler);
// Subscribe to compass heading updates
compass_service_set_heading_filter(DEG_TO_TRIGANGLE(5));
compass_service_subscribe(compass_heading_handler);
// Register with TickTimerService
tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(s_main_window);
  free(nameCallbacks);
  accel_data_service_unsubscribe();
  compass_service_unsubscribe();
  tick_timer_service_unsubscribe();
  game_message_window_destroy();
  pin_window_destroy(pin_window);
  if((strcmp(latlng, "none")) != 0){
    free(latlng);
  }
  if((strcmp(pebbleId, "1234")) != 0){
    free(pebbleId);
  }
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
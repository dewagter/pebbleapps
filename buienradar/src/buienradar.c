#include <pebble.h>

static TextLayer *status_layer;


enum {
  BUI_KEY_STATUS = 0x0,
  BUI_KEY_FETCH = 0x1,
};

///////////////////////////////////////////////////////////
// SEND

static void fetch_buienradar() {
  Tuplet station_tuple = TupletCString(BUI_KEY_FETCH, "here");

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_tuplet(iter, &station_tuple);
  dict_write_end(iter);

  app_message_outbox_send();
  text_layer_set_text(status_layer, "App sent request.");
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App sent request to JS!");
}

///////////////////////////////////////////////////////////
// RECEIVE

static void in_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *status_tuple = dict_find(iter, BUI_KEY_STATUS);

  if (status_tuple) {
    text_layer_set_text(status_layer, status_tuple->value->cstring);
  }
}

static void in_dropped_handler(AppMessageResult reason, void *context) {
  text_layer_set_text(status_layer, "Msg dropped...");
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Dropped!");
}

static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  text_layer_set_text(status_layer, "Msg failed to send.");
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Failed to Send!");
}

static void app_message_init(void) {
  // Register message handlers
  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_failed(out_failed_handler);
  // Init buffers
  app_message_open(64, 64);
}



///////////////////////////////////////////////////////////
// BUTTONS

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(status_layer, "Request BuienRadar");
  fetch_buienradar();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  //text_layer_set_text(status_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  //text_layer_set_text(status_layer, "Down");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

///////////////////////////////////////////////////////////
// WINDOW (UN)LOAD

static Window *window;
static TextLayer* s1_layer;

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s1_layer = text_layer_create((GRect) { .origin = { 0, 62 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(s1_layer, "BuienRadar>");
  text_layer_set_text_alignment(s1_layer, GTextAlignmentRight);
  text_layer_set_font(s1_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s1_layer));

  status_layer = text_layer_create((GRect) { .origin = { 0, 120 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(status_layer, "App Started");
  text_layer_set_font(status_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(status_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(status_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(status_layer);
  text_layer_destroy(s1_layer);
}

///////////////////////////////////////////////////////////
// APP (DE)INIT

static void init(void) {
  window = window_create();
  app_message_init();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

///////////////////////////////////////////////////////////
// MAIN

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}

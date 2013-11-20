#include <pebble.h>

static TextLayer *status_layer;


enum {
  NS_KEY_STATUS = 0x0,
  NS_KEY_TRAIN = 0x1,
  NS_KEY_STATION = 0x2,
};

///////////////////////////////////////////////////////////
// SEND

static void fetch_station(char* Station) {
  Tuplet station_tuple = TupletCString(NS_KEY_STATION, Station);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_tuplet(iter, &station_tuple);
  dict_write_end(iter);

  app_message_outbox_send();
  text_layer_set_text(status_layer, "App sent request.");
}

///////////////////////////////////////////////////////////
// RECEIVE

static void in_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *status_tuple = dict_find(iter, NS_KEY_STATUS);
//  Tuple *train_tuple = dict_find(iter, NS_KEY_TRAIN);

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
  text_layer_set_text(status_layer, "Up=Haarlem/Dn=Delft?");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(status_layer, "Haarlem");
  fetch_station("Haarlem");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(status_layer, "Delft");
  fetch_station("Delft");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}


///////////////////////////////////////////////////////////
// TIME

TextLayer *text_time_layer;

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  // Need to be static because they're used by the system later.
  static char time_text[] = "00:00";

  char *time_format;

  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  strftime(time_text, sizeof(time_text), time_format, tick_time);

  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  text_layer_set_text(text_time_layer, time_text);
}


///////////////////////////////////////////////////////////
// WINDOW (UN)LOAD

static Window *window;

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  TextLayer* s1_layer = text_layer_create((GRect) { .origin = { 0, 3 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(s1_layer, "Haarlem>");
  text_layer_set_text_alignment(s1_layer, GTextAlignmentRight);
  text_layer_set_font(s1_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s1_layer));

  TextLayer* s2_layer = text_layer_create((GRect) { .origin = { 0, 133 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(s2_layer, "Delft>");
  text_layer_set_text_alignment(s2_layer, GTextAlignmentRight);
  text_layer_set_font(s1_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s2_layer));

  status_layer = text_layer_create((GRect) { .origin = { 0, 100 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(status_layer, "Select Station...");
  text_layer_set_font(status_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(status_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(status_layer));

  text_time_layer = text_layer_create(GRect(7, 40, 144-7, 50));
  text_layer_set_font(text_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));

}

static void window_unload(Window *window) {
  text_layer_destroy(status_layer);
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

  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_minute_tick(current_time, MINUTE_UNIT);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
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

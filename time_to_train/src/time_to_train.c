#include "pebble.h"

Window *window;
TextLayer *text_date_layer;
TextLayer *text_time_layer;
TextLayer *text_train_layer;
Layer *line_layer;

void line_layer_update_callback(Layer *layer, GContext* ctx) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  // Need to be static because they're used by the system later.
  static char time_text[] = "00:00";
  static char date_text[] = "Xxxxxxxxx 00";
  static char train_text[] = "Next train in xx minutes";

  int dt = tick_time->tm_min;
  int train = 0;
  int train_h = tick_time->tm_hour;

  if (tick_time->tm_hour < 12)
  {
    train = 29;
  } else {
    train = 20;
  }

  if ((dt > train) && (dt < (train+30)))
  {
    train += 30;
  } else {
    if (dt > train)
    {
      train_h++;
      if (train_h >= 24)
        train_h -= 24;
    }
  }

  dt = train - dt;

  snprintf(train_text, 24, "%d:%d train in %d min", train_h, train, dt);

  char *time_format;


  text_layer_set_text(text_train_layer, train_text);


  // TODO: Only update the date when it's changed.
  strftime(date_text, sizeof(date_text), "%B %e", tick_time);
  text_layer_set_text(text_date_layer, date_text);


  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  strftime(time_text, sizeof(time_text), time_format, tick_time);

  // Kludge to handle lack of non-padded hour format string
  // for twelve hour clock.
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }

  text_layer_set_text(text_time_layer, time_text);
}

void handle_deinit(void) {
  tick_timer_service_unsubscribe();
}

void handle_init(void) {
  window = window_create();
  window_stack_push(window, true /* Animated */);
  window_set_background_color(window, GColorBlack);

  Layer *window_layer = window_get_root_layer(window);

  text_date_layer = text_layer_create(GRect(8, 68+4, 144-8, 168-68));
  text_layer_set_text_color(text_date_layer, GColorWhite);
  text_layer_set_background_color(text_date_layer, GColorClear);
  //text_layer_set_font(text_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_font(text_date_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  layer_add_child(window_layer, text_layer_get_layer(text_date_layer));

  text_time_layer = text_layer_create(GRect(7, 92+4, 144-7, 168-92));
  text_layer_set_text_color(text_time_layer, GColorWhite);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_font(text_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));

  GRect line_frame = GRect(8, 97, 139, 2);
  line_layer = layer_create(line_frame);
  layer_set_update_proc(line_layer, line_layer_update_callback);
  layer_add_child(window_layer, line_layer);

  text_train_layer = text_layer_create(GRect(8, 10, 144-8, 168-68));
  text_layer_set_text_color(text_train_layer, GColorWhite);
  text_layer_set_background_color(text_train_layer, GColorClear);
  //text_layer_set_font(text_train_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_font(text_train_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(text_train_layer));

  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_minute_tick(current_time, MINUTE_UNIT);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}


int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}

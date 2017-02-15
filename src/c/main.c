#include <pebble.h>

//APP_LOG(APP_LOG_LEVEL_DEBUG, "LOG MESSAGE");

// window layers
static Window *main_window;
static Layer *canvas_layer;
static Layer *time_layer;
static TextLayer *number2_layer;
static TextLayer *number3_layer;
static TextLayer *number4_layer;
static TextLayer *number5_layer;
static TextLayer *number6_layer;
static TextLayer *number7_layer;
static TextLayer *number8_layer;
static TextLayer *number9_layer;
static TextLayer *number10_layer;
static TextLayer *number12_layer;
static TextLayer *day_layer;
static TextLayer *date_layer;
static TextLayer *health_layer;

// bluetooth icon
static BitmapLayer *bt_icon_layer;
static GBitmap *bt_icon_bitmap;
bool bt_startup = true;

// health events
static char steps_buffer[9];
static char sleep_buffer[11];
bool sleep_bool;

// saved settings
uint32_t dates_setting  = 0;
uint32_t health_setting = 1;
bool dates_bool;
bool health_bool;

// load date
static char day_buffer[4];
static char date_buffer[3];

// load options
static void load_options() {
  // load date
  if (persist_exists(dates_setting)) {
    char dates_buffer[5];
    persist_read_string(dates_setting, dates_buffer, sizeof(dates_buffer));
    if (strcmp(dates_buffer, "true") == 0) {
      dates_bool = true;
      text_layer_set_text(day_layer,  day_buffer);
      text_layer_set_text(date_layer, date_buffer);
    } else {
      dates_bool = false;
      text_layer_set_text(day_layer,  "");
      text_layer_set_text(date_layer, "");
    }
  } else {
    dates_bool = false;
  }

  // load weather or health
  if (persist_exists(health_setting)) {
    char health_buffer[5];
    persist_read_string(health_setting, health_buffer, sizeof(health_buffer));
    if (strcmp(health_buffer, "true") == 0) {
      health_bool = true;
      if (sleep_bool) {
        if (strlen(sleep_buffer) != 0) {
          text_layer_set_text(health_layer, sleep_buffer);
        } else {
          text_layer_set_text(health_layer, "sleep...");
        }
      } else {
        if (strlen(steps_buffer) != 0) {
          text_layer_set_text(health_layer, steps_buffer);
        } else {
          text_layer_set_text(health_layer, "steps...");
        }
      }
    } else {
      health_bool = false;
      text_layer_set_text(health_layer, "");
    }
  } else {
    health_bool = false;
  }
}

// update options
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // collect options
  Tuple *date_tuple   = dict_find(iterator, MESSAGE_KEY_DATE);
  Tuple *health_tuple = dict_find(iterator, MESSAGE_KEY_HEALTH);

  // save options
  if (date_tuple) {
    char *dates_string = date_tuple->value->cstring;
    persist_write_string(dates_setting, dates_string);
    if (health_tuple) {
      char *health_string = health_tuple->value->cstring;
      persist_write_string(health_setting, health_string);
    }
    load_options();
  }
}

// health event changed
#if defined(PBL_HEALTH)
  static void health_handler(HealthEventType event, void *context) {
    if (event == HealthEventMovementUpdate) {
      sleep_bool = false;
      snprintf(steps_buffer, sizeof(steps_buffer), "%d st", (int)health_service_sum_today(HealthMetricStepCount));
      if (health_bool) {
        text_layer_set_text(health_layer, steps_buffer);
      }
    } else if (event == HealthEventSleepUpdate) {
      sleep_bool = true;
      static int seconds, minutes, hours, remainder;
      static char mins_buffer[4], hour_buffer[4];
      seconds = (int)health_service_sum_today(HealthMetricSleepSeconds);
      hours = seconds / 3600;
      remainder = seconds % 3600;
      minutes = remainder / 60;
      snprintf(hour_buffer, sizeof(hour_buffer), "%dh", (int)hours);
      snprintf(mins_buffer, sizeof(mins_buffer), "%dm", (int)minutes);
      snprintf(sleep_buffer, sizeof(sleep_buffer), "zZZ %s", hour_buffer);
      int length = strlen(sleep_buffer);
      snprintf(sleep_buffer+length, (sizeof sleep_buffer) - length, "%s", mins_buffer);
      if (health_bool) {
        text_layer_set_text(health_layer, sleep_buffer);
      }
    }
  }
#endif

// bluetooth connection change
static void bluetooth_callback(bool connected) {
  layer_set_hidden(bitmap_layer_get_layer(bt_icon_layer), connected);
  if(!bt_startup) {
    vibes_double_pulse();
  }
  bt_startup = false;
}

// update date
static void update_date() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // define date
  strftime(day_buffer,  sizeof(day_buffer),  "%a", tick_time);
  strftime(date_buffer, sizeof(date_buffer), "%d", tick_time);

  // display date
  if (!dates_bool) {
    text_layer_set_text(day_layer,  "");
    text_layer_set_text(date_layer, "");
  } else {
    text_layer_set_text(day_layer,  day_buffer);
    text_layer_set_text(date_layer, date_buffer);
  }
}

// update time
static void update_time(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  // get canvas size
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  int cx = bounds.size.w/2;
  int cy = bounds.size.h/2;

  // draw minute hand
  int32_t minute_angle = TRIG_MAX_ANGLE * t->tm_min / 60;
  const int16_t minute_outside_length = PBL_IF_ROUND_ELSE(cx-19,cx-4);
  const int16_t minute_hand_length = PBL_IF_ROUND_ELSE(cx-20,cx-5);
  GPoint minute_outside = {
    .x = (int16_t)(sin_lookup(minute_angle) * (int32_t)minute_outside_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(minute_angle) * (int32_t)minute_outside_length / TRIG_MAX_RATIO) + center.y,
  };
  GPoint minute_hand = {
    .x = (int16_t)(sin_lookup(minute_angle) * (int32_t)minute_hand_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(minute_angle) * (int32_t)minute_hand_length / TRIG_MAX_RATIO) + center.y,
  };
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_stroke_width(ctx, 11);
  graphics_draw_line(ctx, minute_outside, center);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 9);
  graphics_draw_line(ctx, minute_hand, center);
  
  // draw hour hand
  int32_t hour_angle = TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10)) / (12 * 6);
  const int16_t hour_outside_length = PBL_IF_ROUND_ELSE(cx-44,cx-34);
  const int16_t hour_hand_length = PBL_IF_ROUND_ELSE(cx-45,cx-35);
  const int16_t hour_inside_length = PBL_IF_ROUND_ELSE(cx-55,cx-45);
  GPoint hour_outside = {
    .x = (int16_t)(sin_lookup(hour_angle) * (int32_t)hour_outside_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(hour_angle) * (int32_t)hour_outside_length / TRIG_MAX_RATIO) + center.y,
  };
  GPoint hour_hand = {
    .x = (int16_t)(sin_lookup(hour_angle) * (int32_t)hour_hand_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(hour_angle) * (int32_t)hour_hand_length / TRIG_MAX_RATIO) + center.y,
  };
  GPoint hour_inside = {
    .x = (int16_t)(sin_lookup(hour_angle) * (int32_t)hour_inside_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(hour_angle) * (int32_t)hour_inside_length / TRIG_MAX_RATIO) + center.y,
  };
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_stroke_width(ctx, 11);
  graphics_draw_line(ctx, hour_outside, center);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_stroke_width(ctx, 9);
  graphics_draw_line(ctx, hour_hand, center);
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_stroke_width(ctx, 5);
  graphics_draw_line(ctx, hour_inside, center);

  // draw centre
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(cx,cy), 6);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, GPoint(cx,cy), 3);
}

// refresh date & time
static void mins_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_date();
  layer_mark_dirty(time_layer);
}

// drawing canvas
static void canvas_update_proc(Layer *layer, GContext *ctx) {
  // get canvas size
  GRect bounds = layer_get_bounds(layer);
  int cx = bounds.size.w/2;
  int cy = bounds.size.h/2;
  
  // set definitions
  graphics_context_set_stroke_width(ctx, 1);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  
  // draw 12
  graphics_draw_line(ctx, GPoint(cx-7,PBL_IF_ROUND_ELSE(cy-84,cy-82)),  GPoint(cx-7,PBL_IF_ROUND_ELSE(cy-63,cy-61)));
  graphics_draw_line(ctx, GPoint(cx-8,PBL_IF_ROUND_ELSE(cy-84,cy-82)),  GPoint(cx-8,PBL_IF_ROUND_ELSE(cy-63,cy-61)));
  graphics_draw_line(ctx, GPoint(cx-9,PBL_IF_ROUND_ELSE(cy-84,cy-82)),  GPoint(cx-9,PBL_IF_ROUND_ELSE(cy-63,cy-61)));
  graphics_draw_line(ctx, GPoint(cx-10,PBL_IF_ROUND_ELSE(cy-84,cy-82)), GPoint(cx-10,PBL_IF_ROUND_ELSE(cy-63,cy-61)));
  
  // draw 11
  graphics_draw_line(ctx, GPoint(cx-40,PBL_IF_ROUND_ELSE(cy-76,cy-82)), GPoint(cx-40,PBL_IF_ROUND_ELSE(cy-55,cy-61)));
  graphics_draw_line(ctx, GPoint(cx-41,PBL_IF_ROUND_ELSE(cy-76,cy-82)), GPoint(cx-41,PBL_IF_ROUND_ELSE(cy-55,cy-61)));
  graphics_draw_line(ctx, GPoint(cx-42,PBL_IF_ROUND_ELSE(cy-76,cy-82)), GPoint(cx-42,PBL_IF_ROUND_ELSE(cy-55,cy-61)));
  graphics_draw_line(ctx, GPoint(cx-43,PBL_IF_ROUND_ELSE(cy-76,cy-82)), GPoint(cx-43,PBL_IF_ROUND_ELSE(cy-55,cy-61)));
  graphics_draw_line(ctx, GPoint(cx-29,PBL_IF_ROUND_ELSE(cy-76,cy-82)), GPoint(cx-29,PBL_IF_ROUND_ELSE(cy-55,cy-61)));
  graphics_draw_line(ctx, GPoint(cx-30,PBL_IF_ROUND_ELSE(cy-76,cy-82)), GPoint(cx-30,PBL_IF_ROUND_ELSE(cy-55,cy-61)));
  graphics_draw_line(ctx, GPoint(cx-31,PBL_IF_ROUND_ELSE(cy-76,cy-82)), GPoint(cx-31,PBL_IF_ROUND_ELSE(cy-55,cy-61)));
  graphics_draw_line(ctx, GPoint(cx-32,PBL_IF_ROUND_ELSE(cy-76,cy-82)), GPoint(cx-32,PBL_IF_ROUND_ELSE(cy-55,cy-61)));
  
  // draw 10
  graphics_draw_line(ctx, GPoint(PBL_IF_ROUND_ELSE(17,2),PBL_IF_ROUND_ELSE(cy-46,cy-52)), GPoint(PBL_IF_ROUND_ELSE(17,2),PBL_IF_ROUND_ELSE(cy-25,cy-31)));
  graphics_draw_line(ctx, GPoint(PBL_IF_ROUND_ELSE(18,3),PBL_IF_ROUND_ELSE(cy-46,cy-52)), GPoint(PBL_IF_ROUND_ELSE(18,3),PBL_IF_ROUND_ELSE(cy-25,cy-31)));
  graphics_draw_line(ctx, GPoint(PBL_IF_ROUND_ELSE(19,4),PBL_IF_ROUND_ELSE(cy-46,cy-52)), GPoint(PBL_IF_ROUND_ELSE(19,4),PBL_IF_ROUND_ELSE(cy-25,cy-31)));
  graphics_draw_line(ctx, GPoint(PBL_IF_ROUND_ELSE(20,5),PBL_IF_ROUND_ELSE(cy-46,cy-52)), GPoint(PBL_IF_ROUND_ELSE(20,5),PBL_IF_ROUND_ELSE(cy-25,cy-31)));
  
  // draw 1
  graphics_draw_line(ctx, GPoint(cx+35,PBL_IF_ROUND_ELSE(cy-76,cy-82)), GPoint(cx+35,PBL_IF_ROUND_ELSE(cy-55,cy-61)));
  graphics_draw_line(ctx, GPoint(cx+36,PBL_IF_ROUND_ELSE(cy-76,cy-82)), GPoint(cx+36,PBL_IF_ROUND_ELSE(cy-55,cy-61)));
  graphics_draw_line(ctx, GPoint(cx+37,PBL_IF_ROUND_ELSE(cy-76,cy-82)), GPoint(cx+37,PBL_IF_ROUND_ELSE(cy-55,cy-61)));
  graphics_draw_line(ctx, GPoint(cx+38,PBL_IF_ROUND_ELSE(cy-76,cy-82)), GPoint(cx+38,PBL_IF_ROUND_ELSE(cy-55,cy-61)));

  // draw guidelines
  //int mx = bounds.size.w;
  //int my = bounds.size.h;
  //graphics_draw_line(ctx, GPoint(0,cy), GPoint(mx,cy));
  //graphics_draw_line(ctx, GPoint(cx,0), GPoint(cx,my));
  //graphics_draw_line(ctx, GPoint(cx-36,0), GPoint(cx-36,my));
  //graphics_draw_line(ctx, GPoint(cx+36,0), GPoint(cx+36,my));
  //graphics_draw_line(ctx, GPoint(0,cy-36), GPoint(mx,cy-36));
  //graphics_draw_line(ctx, GPoint(0,cy+36), GPoint(mx,cy+36));
  //graphics_draw_line(ctx, GPoint(0,cy-42), GPoint(mx,cy-42));
  //graphics_draw_line(ctx, GPoint(0,cy+42), GPoint(mx,cy+42));
}

// window load
static void main_window_load(Window *window) {
  // collect window size
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  int mx = bounds.size.w;
  int my = bounds.size.h;
  int cy = bounds.size.h/2;
  
  // drawing canvas
  canvas_layer = layer_create(bounds);
  layer_set_update_proc(canvas_layer, canvas_update_proc);
  layer_add_child(window_layer, canvas_layer);
  
  // numbers layers
  number12_layer = text_layer_create(GRect(+4,PBL_IF_ROUND_ELSE(cy-94,cy-92),mx,my));
  number6_layer  = text_layer_create(GRect(+1,PBL_IF_ROUND_ELSE(cy+52,cy+50),mx,my));
  number3_layer  = text_layer_create(GRect(PBL_IF_ROUND_ELSE(-4,1),cy-21,mx,my));
  number9_layer  = text_layer_create(GRect(PBL_IF_ROUND_ELSE(+5,0),cy-21,mx,my));
  number5_layer  = text_layer_create(GRect(+37,PBL_IF_ROUND_ELSE(cy+42,cy+50),mx,my));
  number7_layer  = text_layer_create(GRect(-35,PBL_IF_ROUND_ELSE(cy+42,cy+50),mx,my));
  number2_layer  = text_layer_create(GRect(PBL_IF_ROUND_ELSE(-15,1),PBL_IF_ROUND_ELSE(cy-56,cy-62),mx,my));
  number10_layer = text_layer_create(GRect(PBL_IF_ROUND_ELSE(+21,6),PBL_IF_ROUND_ELSE(cy-56,cy-62),mx,my));
  number4_layer  = text_layer_create(GRect(PBL_IF_ROUND_ELSE(-15,1),PBL_IF_ROUND_ELSE(cy+15,cy+21),mx,my));
  number8_layer  = text_layer_create(GRect(PBL_IF_ROUND_ELSE(+16,0),PBL_IF_ROUND_ELSE(cy+15,cy+21),mx,my));
  text_layer_set_background_color(number2_layer,  GColorClear);
  text_layer_set_background_color(number3_layer,  GColorClear);
  text_layer_set_background_color(number4_layer,  GColorClear);
  text_layer_set_background_color(number5_layer,  GColorClear);
  text_layer_set_background_color(number6_layer,  GColorClear);
  text_layer_set_background_color(number7_layer,  GColorClear);
  text_layer_set_background_color(number8_layer,  GColorClear);
  text_layer_set_background_color(number9_layer,  GColorClear);
  text_layer_set_background_color(number10_layer, GColorClear);
  text_layer_set_background_color(number12_layer, GColorClear);
  text_layer_set_text_color(number2_layer,  GColorWhite);
  text_layer_set_text_color(number3_layer,  GColorWhite);
  text_layer_set_text_color(number4_layer,  GColorWhite);
  text_layer_set_text_color(number5_layer,  GColorWhite);
  text_layer_set_text_color(number6_layer,  GColorWhite);
  text_layer_set_text_color(number7_layer,  GColorWhite);
  text_layer_set_text_color(number8_layer,  GColorWhite);
  text_layer_set_text_color(number9_layer,  GColorWhite);
  text_layer_set_text_color(number10_layer, GColorWhite);
  text_layer_set_text_color(number12_layer, GColorWhite);
  text_layer_set_text_alignment(number2_layer,  GTextAlignmentRight);
  text_layer_set_text_alignment(number3_layer,  GTextAlignmentRight);
  text_layer_set_text_alignment(number4_layer,  GTextAlignmentRight);
  text_layer_set_text_alignment(number5_layer,  GTextAlignmentCenter);
  text_layer_set_text_alignment(number6_layer,  GTextAlignmentCenter);
  text_layer_set_text_alignment(number7_layer,  GTextAlignmentCenter);
  text_layer_set_text_alignment(number8_layer,  GTextAlignmentLeft);
  text_layer_set_text_alignment(number9_layer,  GTextAlignmentLeft);
  text_layer_set_text_alignment(number10_layer, GTextAlignmentLeft);
  text_layer_set_text_alignment(number12_layer, GTextAlignmentCenter);
  text_layer_set_font(number2_layer,  fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS));
  text_layer_set_font(number3_layer,  fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS));
  text_layer_set_font(number4_layer,  fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS));
  text_layer_set_font(number5_layer,  fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS));
  text_layer_set_font(number6_layer,  fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS));
  text_layer_set_font(number7_layer,  fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS));
  text_layer_set_font(number8_layer,  fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS));
  text_layer_set_font(number9_layer,  fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS));
  text_layer_set_font(number10_layer, fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS));
  text_layer_set_font(number12_layer, fonts_get_system_font(FONT_KEY_LECO_32_BOLD_NUMBERS));
  text_layer_set_text(number2_layer,  "2");
  text_layer_set_text(number3_layer,  "3");
  text_layer_set_text(number4_layer,  "4");
  text_layer_set_text(number5_layer,  "5");
  text_layer_set_text(number6_layer,  "6");
  text_layer_set_text(number7_layer,  "7");
  text_layer_set_text(number8_layer,  "8");
  text_layer_set_text(number9_layer,  "9");
  text_layer_set_text(number10_layer, "0");
  text_layer_set_text(number12_layer, "2");
  layer_add_child(window_layer, text_layer_get_layer(number2_layer));
  layer_add_child(window_layer, text_layer_get_layer(number3_layer));
  layer_add_child(window_layer, text_layer_get_layer(number4_layer));
  layer_add_child(window_layer, text_layer_get_layer(number5_layer));
  layer_add_child(window_layer, text_layer_get_layer(number6_layer));
  layer_add_child(window_layer, text_layer_get_layer(number7_layer));
  layer_add_child(window_layer, text_layer_get_layer(number8_layer));
  layer_add_child(window_layer, text_layer_get_layer(number9_layer));
  layer_add_child(window_layer, text_layer_get_layer(number10_layer));
  layer_add_child(window_layer, text_layer_get_layer(number12_layer));

  // date layers
  day_layer  = text_layer_create(GRect(PBL_IF_ROUND_ELSE(48,38),cy-15,mx,my));
  date_layer = text_layer_create(GRect(PBL_IF_ROUND_ELSE(48,38),cy-3, mx,my));
  text_layer_set_background_color(day_layer, GColorClear);
  text_layer_set_background_color(date_layer, GColorClear);
  text_layer_set_text_color(day_layer, GColorWhite);
  text_layer_set_text_color(date_layer, GColorWhite);
  text_layer_set_text_alignment(day_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
  text_layer_set_font(day_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(day_layer));
  layer_add_child(window_layer, text_layer_get_layer(date_layer));
   
  // bluetooth layer
  bt_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BT_ICON);
  bt_icon_layer = bitmap_layer_create(GRect(0,-34,mx,my));
  bitmap_layer_set_bitmap(bt_icon_layer, bt_icon_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(bt_icon_layer));
  bluetooth_callback(connection_service_peek_pebble_app_connection());

  // health layer
  health_layer = text_layer_create(GRect(0,PBL_IF_ROUND_ELSE(cy+27,cy+33),mx,my));
  text_layer_set_background_color(health_layer, GColorClear);
  text_layer_set_text_color(health_layer, GColorWhite);
  text_layer_set_text_alignment(health_layer, GTextAlignmentCenter);
  text_layer_set_font(health_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(health_layer));
  #if defined(PBL_HEALTH)
    if(health_service_events_subscribe(health_handler, NULL)) {
      HealthActivityMask activities = health_service_peek_current_activities();
      if((activities & HealthActivitySleep) || (activities & HealthActivityRestfulSleep)) {
        health_handler(HealthEventSleepUpdate, NULL);
      } else {
        health_handler(HealthEventMovementUpdate, NULL);
      }
    }
  #endif
  
  // time layer
  time_layer = layer_create(bounds);
  layer_set_update_proc(time_layer, update_time);
  layer_add_child(window_layer, time_layer);
}

// window unload
static void main_window_unload(Window *window) {
  // unsubscribe from events
  #if defined(PBL_HEALTH)
    health_service_events_unsubscribe();
  #endif
  // destroy image layers
  bitmap_layer_destroy(bt_icon_layer);
  gbitmap_destroy(bt_icon_bitmap);
  // destroy text layers
  text_layer_destroy(health_layer);
  text_layer_destroy(date_layer);
  text_layer_destroy(day_layer);
  text_layer_destroy(number12_layer);
  text_layer_destroy(number10_layer);
  text_layer_destroy(number9_layer);
  text_layer_destroy(number8_layer);
  text_layer_destroy(number7_layer);
  text_layer_destroy(number6_layer);
  text_layer_destroy(number5_layer);
  text_layer_destroy(number4_layer);
  text_layer_destroy(number3_layer);
  text_layer_destroy(number2_layer);
  // destroy canvas layers
  layer_destroy(time_layer);
  layer_destroy(canvas_layer);
}

// init
static void init() {
  // create window
  main_window = window_create();
  window_set_background_color(main_window, GColorBlack);

  // load/unload window
  window_set_window_handlers(main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  
  // send window to screen
  window_stack_push(main_window, true);
  
  // load options
  load_options();

  // update date
  update_date();

  // subscribe to minute service
  tick_timer_service_subscribe(MINUTE_UNIT, mins_tick_handler);
  
  // check bluetooth connection
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_callback
  });

  // update options/weather
  app_message_register_inbox_received(inbox_received_callback);
  app_message_open(64,0);  
}

// deinit
static void deinit() {
  // unsubscribe from events
  connection_service_unsubscribe();
  tick_timer_service_unsubscribe();
  // destroy window
  window_destroy(main_window);
}

// main
int main(void) {
  init();
  app_event_loop();
  deinit();
}
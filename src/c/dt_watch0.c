#include <pebble.h>
#include <pebble_fonts.h>

// Settings
#define SETTINGS_KEY 1

// constants
#define TIME_STR_LEN (5+1)
#define DATE_STR_LEN (9+1)
#define DATE_STRFTIME_LEN 12
#define LECO_53_HEIGHT 53
#define LECO_25_HEIGHT 25

// Define settings struct
typedef struct ClaySettings {
  GColor TimeFontColor;
  GColor DateFontColor;
  GColor TimeDateBackgroundColor;
  GColor InfoBackgroundColor;
  char DateStrftimeStr[DATE_STRFTIME_LEN];
} ClaySettings;


// windows
static Window *s_window;
// Layers
static TextLayer *s_time_text_layer;
static TextLayer *s_date_text_layer;
static TextLayer *s_info_text_layer;
// Resources
static GFont leco_25;
static GFont leco_53;
// module variables
static ClaySettings settings;
static char s_time_buffer[TIME_STR_LEN];
static char s_date_buffer[DATE_STR_LEN];


static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_unobstructed_bounds(window_layer);

  s_time_text_layer = text_layer_create(GRect(0, 0, bounds.size.w, LECO_53_HEIGHT));
  text_layer_set_font(s_time_text_layer, leco_53);
  text_layer_set_text(s_time_text_layer, s_time_buffer);
  text_layer_set_text_alignment(s_time_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_text_layer));

  s_date_text_layer = text_layer_create(GRect(0, LECO_53_HEIGHT, bounds.size.w, LECO_25_HEIGHT + 4));
  text_layer_set_font(s_date_text_layer, leco_25);
  text_layer_set_text(s_date_text_layer, s_date_buffer);
  text_layer_set_text_alignment(s_date_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_date_text_layer));
  
  s_info_text_layer = text_layer_create(GRect(0,
                                              LECO_53_HEIGHT + LECO_25_HEIGHT + 4,
                                              bounds.size.w,
                                              bounds.size.h-(LECO_53_HEIGHT + LECO_25_HEIGHT + 4)));
  text_layer_set_font(s_info_text_layer, leco_25);
  text_layer_set_text(s_info_text_layer, "INFO!\nFuture\nFeature");
  text_layer_set_text_alignment(s_info_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_info_text_layer));  
}

static void prv_window_unload(Window *window) {
  text_layer_destroy(s_time_text_layer);
  text_layer_destroy(s_date_text_layer);
  text_layer_destroy(s_info_text_layer);
}

static void update_time_str(struct tm *tick_time) {
  strftime(s_time_buffer,
           sizeof(s_time_buffer),
           "%I:%M",
           tick_time);
}

void toupper_str(char *str)
{
  char character;
  while (*str != '\0') {
    character = (unsigned char)*str;
    if (('a' <= character) && (character <= 'z')) {
      character -= 'a' - 'A';
      *str = character;
    }
    str++;
  }
}

static void update_date_str(struct tm *tick_time) {
  strftime(s_date_buffer,
           sizeof(s_date_buffer),
           settings.DateStrftimeStr,
           tick_time);
  toupper_str(s_date_buffer);
}

static void update_time(struct tm *tick_time) {
  update_time_str(tick_time);
  text_layer_set_text(s_time_text_layer, s_time_buffer);
}

static void update_date(struct tm *tick_time) {
  update_date_str(tick_time);
  text_layer_set_text(s_date_text_layer, s_date_buffer);  
}

struct tm *now(void) {
  const time_t now = time(NULL);
  return localtime(&now);
}

static void apply_settings(void) {
  text_layer_set_text_color(s_time_text_layer, settings.TimeFontColor);
  text_layer_set_text_color(s_date_text_layer, settings.DateFontColor);
  text_layer_set_background_color(s_time_text_layer, settings.TimeDateBackgroundColor);
  text_layer_set_background_color(s_date_text_layer, settings.TimeDateBackgroundColor);
  text_layer_set_background_color(s_info_text_layer, settings.InfoBackgroundColor);
  update_date(now());
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
  
  if ((units_changed & DAY_UNIT) || (s_date_buffer[0] == 'X')) {
    update_date(tick_time);
  }
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "inbox_received_handler!");
  
  Tuple *time_font_color_t = dict_find(iter, MESSAGE_KEY_TimeFontColor);
  if(time_font_color_t) {
    settings.TimeFontColor = GColorFromHEX(time_font_color_t->value->int32);
  }

  Tuple *date_font_color_t = dict_find(iter, MESSAGE_KEY_DateFontColor);
  if(date_font_color_t) {
    settings.DateFontColor = GColorFromHEX(date_font_color_t->value->int32);
  }

  Tuple *td_bg_color_t = dict_find(iter, MESSAGE_KEY_TimeDateBackgroundColor);
  if(td_bg_color_t) {
    settings.TimeDateBackgroundColor = GColorFromHEX(td_bg_color_t->value->int32);
  }

  Tuple *info_bg_color_t = dict_find(iter, MESSAGE_KEY_InfoBackgroundColor);
  if(info_bg_color_t) {
    settings.InfoBackgroundColor = GColorFromHEX(info_bg_color_t->value->int32);
  }

  Tuple *date_strftime_t = dict_find(iter, MESSAGE_KEY_DateStrftimeStr);
  if(date_strftime_t) {
    strncpy(settings.DateStrftimeStr, date_strftime_t->value->cstring, sizeof(settings.DateStrftimeStr));
  }
  apply_settings();
}


static void prv_init(void) {
  // settings_init()
  settings.TimeFontColor = GColorWhite;
  settings.DateFontColor = GColorWhite;
  settings.TimeDateBackgroundColor = GColorBlack;
  settings.InfoBackgroundColor = GColorBlack;
  strncpy(settings.DateStrftimeStr, "%a %d%b", sizeof(settings.DateStrftimeStr));
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
  
  // message_init() AppMessage connection
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(128, 128);

  // time and date initial values
  update_time_str(now());
  update_date_str(now());

  // fonts
  leco_53 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_LECO_FONT_53));
  leco_25 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_LECO_FONT_25));
  
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = prv_window_load,
    .unload = prv_window_unload,
  });
  const bool animated = true;
  window_stack_push(s_window, animated);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);  
  apply_settings();
}

// Save the settings to persistent storage
static void save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
}

static void prv_deinit(void) {
  save_settings();
  window_destroy(s_window);
}

int main(void) {
  prv_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);

  app_event_loop();
  prv_deinit();
}

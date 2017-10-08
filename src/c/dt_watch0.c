#include <stdbool.h>
#include <pebble.h>
#include <pebble_fonts.h>
#include <pebble-battery-bar/pebble-battery-bar.h>
#include <pebble-events/pebble-events.h>

// Settings
#define SETTINGS_KEY 1
#define WEATHER_KEY 2

// constants
#define TIME_STR_LEN (5+1)
#define DATE_STR_LEN (9+1)
#define DATE_STRFTIME_LEN 12
#define LECO_14_HEIGHT 14
#define LECO_16_HEIGHT 16
#define LECO_25_HEIGHT 25
#define LECO_53_HEIGHT 53

// Define settings struct
typedef struct ClaySettings {
  GColor TimeFontColor;
  GColor DateFontColor;
  GColor WeatherFontColor;
  GColor TimeDateBackgroundColor;
  GColor ForecastBackgroundColor;
  char DateStrftimeStr[DATE_STRFTIME_LEN];
} ClaySettings;

typedef struct WeatherState {
  time_t timestamp;
  int weather_icon_id[3];
  char weather_time_buffer[3][5];
  char weather_temp_buffer[3][5];
} WeatherState;

// windows
static Window *s_window;
// handle
static EventHandle s_bt_handle;
// Layers
static TextLayer *s_utc_time_text_layer;
static TextLayer *s_time_text_layer;
static TextLayer *s_date_text_layer;
static TextLayer *s_forecast_text_layer;
static TextLayer *s_weather_time_text_layer[3];
static TextLayer *s_weather_temp_text_layer[3];
static BitmapLayer *s_weather_bitmap_layer[3];
static BitmapLayer *s_bt_icon_layer;
static BatteryBarLayer *s_battery_layer;
// Resources
static GFont leco_14;
static GFont leco_16;
static GFont leco_25;
static GFont leco_53;
static GBitmap *bt_icon_disconnected;
static GBitmap *weather_chanceflurries_bitmap;
static GBitmap *weather_chancerain_bitmap;
static GBitmap *weather_chancesleet_bitmap;
static GBitmap *weather_chancesnow_bitmap;
static GBitmap *weather_chancetstorms_bitmap;
static GBitmap *weather_clear_bitmap;
static GBitmap *weather_cloudy_bitmap;
static GBitmap *weather_flurries_bitmap;
static GBitmap *weather_fog_bitmap;
static GBitmap *weather_hazy_bitmap;
static GBitmap *weather_mostlycloudy_bitmap;
static GBitmap *weather_mostlysunny_bitmap;
static GBitmap *weather_nt_chanceflurries_bitmap;
static GBitmap *weather_nt_chancerain_bitmap;
static GBitmap *weather_nt_chancesleet_bitmap;
static GBitmap *weather_nt_chancesnow_bitmap;
static GBitmap *weather_nt_chancetstorms_bitmap;
static GBitmap *weather_nt_clear_bitmap;
static GBitmap *weather_nt_cloudy_bitmap;
static GBitmap *weather_nt_flurries_bitmap;
static GBitmap *weather_nt_fog_bitmap;
static GBitmap *weather_nt_hazy_bitmap;
static GBitmap *weather_nt_mostlycloudy_bitmap;
static GBitmap *weather_nt_mostlysunny_bitmap;
static GBitmap *weather_nt_partlycloudy_bitmap;
static GBitmap *weather_nt_partlysunny_bitmap;
static GBitmap *weather_nt_rain_bitmap;
static GBitmap *weather_nt_sleet_bitmap;
static GBitmap *weather_nt_snow_bitmap;
static GBitmap *weather_nt_sunny_bitmap;
static GBitmap *weather_nt_tstorms_bitmap;
static GBitmap *weather_partlycloudy_bitmap;
static GBitmap *weather_partlysunny_bitmap;
static GBitmap *weather_rain_bitmap;
static GBitmap *weather_sleet_bitmap;
static GBitmap *weather_snow_bitmap;
static GBitmap *weather_sunny_bitmap;
static GBitmap *weather_tstorms_bitmap;
static GBitmap *weather_unknown_bitmap;

// module variables
static ClaySettings settings;
static char s_utc_time_buffer[TIME_STR_LEN];
static char s_time_buffer[TIME_STR_LEN];
static char s_date_buffer[DATE_STR_LEN];
static WeatherState s_weather_state;


static bool s_fetched_initial_weather = false;
static bool s_jsready = false;

// Weather

#define WEATHER_ICON_ID_chanceflurries       1
#define WEATHER_ICON_ID_chancerain           2
#define WEATHER_ICON_ID_chancesleet          3
#define WEATHER_ICON_ID_chancesnow           4
#define WEATHER_ICON_ID_chancetstorms        5
#define WEATHER_ICON_ID_clear                6
#define WEATHER_ICON_ID_cloudy               7
#define WEATHER_ICON_ID_flurries             8
#define WEATHER_ICON_ID_fog                  9
#define WEATHER_ICON_ID_hazy                 10
#define WEATHER_ICON_ID_mostlycloudy         11
#define WEATHER_ICON_ID_mostlysunny          12
#define WEATHER_ICON_ID_nt_chanceflurries    13
#define WEATHER_ICON_ID_nt_chancerain        14
#define WEATHER_ICON_ID_nt_chancesleet       15
#define WEATHER_ICON_ID_nt_chancesnow        16
#define WEATHER_ICON_ID_nt_chancetstorms     17
#define WEATHER_ICON_ID_nt_clear             18
#define WEATHER_ICON_ID_nt_cloudy            19
#define WEATHER_ICON_ID_nt_flurries          20
#define WEATHER_ICON_ID_nt_fog               21
#define WEATHER_ICON_ID_nt_hazy              22
#define WEATHER_ICON_ID_nt_mostlycloudy      23
#define WEATHER_ICON_ID_nt_mostlysunny       24
#define WEATHER_ICON_ID_nt_partlycloudy      25
#define WEATHER_ICON_ID_nt_partlysunny       26
#define WEATHER_ICON_ID_nt_rain              27
#define WEATHER_ICON_ID_nt_sleet             28
#define WEATHER_ICON_ID_nt_snow              29
#define WEATHER_ICON_ID_nt_sunny             30
#define WEATHER_ICON_ID_nt_tstorms           31
#define WEATHER_ICON_ID_partlycloudy         32
#define WEATHER_ICON_ID_partlysunny          33
#define WEATHER_ICON_ID_rain                 34
#define WEATHER_ICON_ID_sleet                35
#define WEATHER_ICON_ID_snow                 36
#define WEATHER_ICON_ID_sunny                37
#define WEATHER_ICON_ID_tstorms              38
#define WEATHER_ICON_ID_unknown              39

GBitmap *map_weather_id_to_bitmap(int fct) {
  switch (fct) {
  case WEATHER_ICON_ID_chanceflurries:    return weather_chanceflurries_bitmap;
  case WEATHER_ICON_ID_chancerain:        return weather_chancerain_bitmap;
  case WEATHER_ICON_ID_chancesleet:       return weather_chancesleet_bitmap;
  case WEATHER_ICON_ID_chancesnow:        return weather_chancesnow_bitmap;
  case WEATHER_ICON_ID_chancetstorms:     return weather_chancetstorms_bitmap;
  case WEATHER_ICON_ID_clear:             return weather_clear_bitmap;
  case WEATHER_ICON_ID_cloudy:            return weather_cloudy_bitmap;
  case WEATHER_ICON_ID_flurries:          return weather_flurries_bitmap;
  case WEATHER_ICON_ID_fog:               return weather_fog_bitmap;
  case WEATHER_ICON_ID_hazy:              return weather_hazy_bitmap;
  case WEATHER_ICON_ID_mostlycloudy:      return weather_mostlycloudy_bitmap;
  case WEATHER_ICON_ID_mostlysunny:       return weather_mostlysunny_bitmap;
  case WEATHER_ICON_ID_nt_chanceflurries: return weather_nt_chanceflurries_bitmap;
  case WEATHER_ICON_ID_nt_chancerain:     return weather_nt_chancerain_bitmap;
  case WEATHER_ICON_ID_nt_chancesleet:    return weather_nt_chancesleet_bitmap;
  case WEATHER_ICON_ID_nt_chancesnow:     return weather_nt_chancesnow_bitmap;
  case WEATHER_ICON_ID_nt_chancetstorms:  return weather_nt_chancetstorms_bitmap;
  case WEATHER_ICON_ID_nt_clear:          return weather_nt_clear_bitmap;
  case WEATHER_ICON_ID_nt_cloudy:         return weather_nt_cloudy_bitmap;
  case WEATHER_ICON_ID_nt_flurries:       return weather_nt_flurries_bitmap;
  case WEATHER_ICON_ID_nt_fog:            return weather_nt_fog_bitmap;
  case WEATHER_ICON_ID_nt_hazy:           return weather_nt_hazy_bitmap;
  case WEATHER_ICON_ID_nt_mostlycloudy:   return weather_nt_mostlycloudy_bitmap;
  case WEATHER_ICON_ID_nt_mostlysunny:    return weather_nt_mostlysunny_bitmap;
  case WEATHER_ICON_ID_nt_partlycloudy:   return weather_nt_partlycloudy_bitmap;
  case WEATHER_ICON_ID_nt_partlysunny:    return weather_nt_partlysunny_bitmap;
  case WEATHER_ICON_ID_nt_rain:           return weather_nt_rain_bitmap;
  case WEATHER_ICON_ID_nt_sleet:          return weather_nt_sleet_bitmap;
  case WEATHER_ICON_ID_nt_snow:           return weather_nt_snow_bitmap;
  case WEATHER_ICON_ID_nt_sunny:          return weather_nt_sunny_bitmap;
  case WEATHER_ICON_ID_nt_tstorms:        return weather_nt_tstorms_bitmap;
  case WEATHER_ICON_ID_partlycloudy:      return weather_partlycloudy_bitmap;
  case WEATHER_ICON_ID_partlysunny:       return weather_partlysunny_bitmap;
  case WEATHER_ICON_ID_rain:              return weather_rain_bitmap;
  case WEATHER_ICON_ID_sleet:             return weather_sleet_bitmap;
  case WEATHER_ICON_ID_snow:              return weather_snow_bitmap;
  case WEATHER_ICON_ID_sunny:             return weather_sunny_bitmap;
  case WEATHER_ICON_ID_tstorms:           return weather_tstorms_bitmap;
  case WEATHER_ICON_ID_unknown:           return weather_unknown_bitmap;
  default:                                return weather_unknown_bitmap;
  }
}

static void display_weather_state(void) {
  int idx;
  for (idx=0; idx < 3; idx++) {
    text_layer_set_text(s_weather_time_text_layer[idx], s_weather_state.weather_time_buffer[idx]);
    text_layer_set_text(s_weather_temp_text_layer[idx], s_weather_state.weather_temp_buffer[idx]);
    bitmap_layer_set_bitmap(s_weather_bitmap_layer[idx],
                            map_weather_id_to_bitmap(s_weather_state.weather_icon_id[idx]));
  }
}

static void fetch_weather(void) {
  DictionaryIterator *out_iter;
  AppMessageResult result = app_message_outbox_begin(&out_iter);
  // APP_LOG(APP_LOG_LEVEL_DEBUG, "fetch_weather");

  if(result == APP_MSG_OK) {
    // Construct the message
    dict_write_uint8(out_iter, MESSAGE_KEY_FetchWeather, 1);
  } else {
    // The outbox cannot be used right now
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing the outbox: %d", (int)result);
  }

  // Send this message
  result = app_message_outbox_send();

  // Check the result
  if(result != APP_MSG_OK) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending the outbox: %d", (int)result);
  }
}


static void set_default_weather_state(void) {
  int idx;
  s_weather_state.timestamp = 0;
  for (idx=0; idx < 3; idx++) {
    s_weather_state.weather_icon_id[idx] = 39;
    strncpy(s_weather_state.weather_time_buffer[idx], "TBD\0", sizeof(s_weather_state.weather_time_buffer[idx]));
    strncpy(s_weather_state.weather_temp_buffer[idx], "TBD\0", sizeof(s_weather_state.weather_temp_buffer[idx]));
  }

}

#define timestamp_age(timestamp) ((time(NULL) - timestamp))

static void safe_fetch_weather(void) {
  // APP_LOG(APP_LOG_LEVEL_DEBUG, "safe_fetch_weather");
  if (timestamp_age(s_weather_state.timestamp) < 15 * SECONDS_PER_MINUTE) {
    // if ((time(NULL) - s_weather_state.timestamp) < 10) {
    // APP_LOG(APP_LOG_LEVEL_DEBUG, "using old data");
    // APP_LOG(APP_LOG_LEVEL_DEBUG, "%d %d ", time(NULL), s_weather_state.timestamp);
    display_weather_state();
    return;
  }
  fetch_weather();
}

#define TIME_TEXT_LAYER_HEIGHT (LECO_53_HEIGHT + 6)
#define DATE_TEXT_LAYER_HEIGHT (LECO_25_HEIGHT + 6)
#define FORECAST_LAYER_YSTART  (TIME_TEXT_LAYER_HEIGHT + DATE_TEXT_LAYER_HEIGHT)
#define WEATHER_TIME_YSTART    (FORECAST_LAYER_YSTART)
#define WEATHER_BITMAP_YSTART  (FORECAST_LAYER_YSTART)
static void prv_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  //GRect bounds = layer_get_unobstructed_bounds(window_layer);
  GRect bounds = layer_get_bounds(window_layer);

  // time
  s_time_text_layer = text_layer_create(GRect(0, 3, bounds.size.w, TIME_TEXT_LAYER_HEIGHT));
  text_layer_set_font(s_time_text_layer, leco_53);
  text_layer_set_text(s_time_text_layer, s_time_buffer);
  text_layer_set_text_alignment(s_time_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_time_text_layer));

  // mini utc time
  s_utc_time_text_layer = text_layer_create(GRect(3, -2, 60, LECO_14_HEIGHT));
  text_layer_set_font(s_utc_time_text_layer, leco_14);
  //  text_layer_set_font(s_utc_time_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_09));
  text_layer_set_text(s_utc_time_text_layer, s_utc_time_buffer);
  text_layer_set_text_alignment(s_utc_time_text_layer, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(s_utc_time_text_layer));

  // battery bar, added after time so time does not mask it
  s_battery_layer = battery_bar_layer_create();
  layer_add_child(window_layer, s_battery_layer);
  battery_bar_set_percent_hidden(true);
  battery_bar_set_position(GPoint(41, 2));

  // bt icon
  s_bt_icon_layer = bitmap_layer_create(GRect(bounds.size.w-16,
                                              0,
                                              16,
                                              14));
  bitmap_layer_set_compositing_mode(s_bt_icon_layer, GCompOpSet);
  bitmap_layer_set_background_color(s_bt_icon_layer, GColorWhite);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bt_icon_layer));
  bitmap_layer_set_bitmap(s_bt_icon_layer, bt_icon_disconnected);
  layer_mark_dirty((Layer *)s_bt_icon_layer);

  // date
  s_date_text_layer = text_layer_create(GRect(0, TIME_TEXT_LAYER_HEIGHT, bounds.size.w, DATE_TEXT_LAYER_HEIGHT));
  text_layer_set_font(s_date_text_layer, leco_25);
  text_layer_set_text(s_date_text_layer, s_date_buffer);
  text_layer_set_text_alignment(s_date_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_date_text_layer));

  // forecast
  s_forecast_text_layer = text_layer_create(GRect(0,
                                              FORECAST_LAYER_YSTART,
                                              bounds.size.w,
                                              bounds.size.h-(LECO_53_HEIGHT + LECO_25_HEIGHT + 4)));
  //text_layer_set_font(s_forecast_text_layer, leco_25);
  //text_layer_set_text(s_forecast_text_layer, "FORECAST!\nFuture\nFeature");
  text_layer_set_text_alignment(s_forecast_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_forecast_text_layer));

  int idx;
  int WEATHER_TEMP_YSTART = bounds.size.h - LECO_16_HEIGHT - 4;
  for (idx=0; idx < 3; idx++) {
    // weather times
    s_weather_time_text_layer[idx] = text_layer_create(GRect(idx * 48,
                                                             WEATHER_TIME_YSTART,
                                                             48,
                                                             LECO_16_HEIGHT));
    text_layer_set_text_alignment(s_weather_time_text_layer[idx], GTextAlignmentCenter);
    text_layer_set_font(s_weather_time_text_layer[idx], leco_16);
    layer_add_child(window_layer, text_layer_get_layer(s_weather_time_text_layer[idx]));
    // weather temps
    s_weather_temp_text_layer[idx] = text_layer_create(GRect(idx * 48,
                                                             WEATHER_TEMP_YSTART,
                                                             48,
                                                             LECO_16_HEIGHT));
    text_layer_set_text_alignment(s_weather_temp_text_layer[idx], GTextAlignmentCenter);
    text_layer_set_font(s_weather_temp_text_layer[idx], leco_16);
    layer_add_child(window_layer, text_layer_get_layer(s_weather_temp_text_layer[idx]));
    // weather bitmaps
    s_weather_bitmap_layer[idx] = bitmap_layer_create(GRect(idx * 48,
                                                            WEATHER_BITMAP_YSTART,
                                                            48,
                                                            bounds.size.h-FORECAST_LAYER_YSTART));
    bitmap_layer_set_compositing_mode(s_weather_bitmap_layer[idx], GCompOpSet);
    layer_add_child(window_layer, bitmap_layer_get_layer(s_weather_bitmap_layer[idx]));
  }
}

static void prv_window_unload(Window *window) {
  battery_bar_layer_destroy(s_battery_layer);
  text_layer_destroy(s_time_text_layer);
  bitmap_layer_destroy(s_bt_icon_layer);
  text_layer_destroy(s_utc_time_text_layer);
  text_layer_destroy(s_date_text_layer);
  text_layer_destroy(s_forecast_text_layer);
  int idx;
  for (idx=0; idx < 3; idx++) {
    text_layer_destroy(s_weather_time_text_layer[idx]);
    text_layer_destroy(s_weather_temp_text_layer[idx]);
    bitmap_layer_destroy(s_weather_bitmap_layer[idx]);
  }
}

static void update_time_str(struct tm *tick_time) {
  strftime(s_time_buffer,
           sizeof(s_time_buffer),
           clock_is_24h_style() ? "%H:%M" : "%I:%M",
           tick_time);

  time_t temp = time(NULL);
  struct tm *utc_time = gmtime(&temp);
  strftime(s_utc_time_buffer,
           sizeof(s_utc_time_buffer),
           "%H%MZ",
           utc_time);
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
  text_layer_set_text(s_utc_time_text_layer, s_utc_time_buffer);
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
  window_set_background_color(s_window, settings.TimeDateBackgroundColor);
  text_layer_set_text_color(s_utc_time_text_layer, settings.TimeFontColor);
  text_layer_set_text_color(s_time_text_layer, settings.TimeFontColor);
  text_layer_set_text_color(s_date_text_layer, settings.DateFontColor);
  text_layer_set_background_color(s_utc_time_text_layer, settings.TimeDateBackgroundColor);
  text_layer_set_background_color(s_time_text_layer, settings.TimeDateBackgroundColor);
  text_layer_set_background_color(s_date_text_layer, settings.TimeDateBackgroundColor);
  text_layer_set_background_color(s_forecast_text_layer, settings.ForecastBackgroundColor);
  int idx;
  for (idx=0; idx < 3; idx++) {
    text_layer_set_background_color(s_weather_time_text_layer[idx], settings.ForecastBackgroundColor);
    text_layer_set_background_color(s_weather_temp_text_layer[idx], settings.ForecastBackgroundColor);
    text_layer_set_text_color(s_weather_time_text_layer[idx], settings.WeatherFontColor);
    text_layer_set_text_color(s_weather_temp_text_layer[idx], settings.WeatherFontColor);
  }
  update_date(now());
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time(tick_time);
  if (((tick_time->tm_min % 30 == 0 && (units_changed & MINUTE_UNIT))) ||
      (!s_fetched_initial_weather)) {
    if (s_jsready) {
      safe_fetch_weather();
    }
  }

  if ((units_changed & DAY_UNIT) || (s_date_buffer[0] == 'X')) {
    update_date(tick_time);
  }
}

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  // APP_LOG(APP_LOG_LEVEL_DEBUG, "inbox_received_handler!");
  bool settings_updated = false;
  bool weather_updated = false;
  int idx;

  Tuple *ready_tuple = dict_find(iter, MESSAGE_KEY_JSReady);
  if(ready_tuple) {
    s_jsready = true;
    // PebbleKit JS is ready! Safe to send messages
    if (!s_fetched_initial_weather) {
      safe_fetch_weather();
    }
  }

  Tuple *time_font_color_t = dict_find(iter, MESSAGE_KEY_PrefTimeFontColor);
  if(time_font_color_t) {
    settings.TimeFontColor = GColorFromHEX(time_font_color_t->value->int32);
    settings_updated = true;
  }

  Tuple *date_font_color_t = dict_find(iter, MESSAGE_KEY_PrefDateFontColor);
  if(date_font_color_t) {
    settings.DateFontColor = GColorFromHEX(date_font_color_t->value->int32);
    settings_updated = true;
  }

  Tuple *weather_font_color_t = dict_find(iter, MESSAGE_KEY_PrefWeatherFontColor);
  if(weather_font_color_t) {
    settings.WeatherFontColor = GColorFromHEX(weather_font_color_t->value->int32);
    settings_updated = true;
  }

  Tuple *td_bg_color_t = dict_find(iter, MESSAGE_KEY_PrefTimeDateBackgroundColor);
  if(td_bg_color_t) {
    settings.TimeDateBackgroundColor = GColorFromHEX(td_bg_color_t->value->int32);
    settings_updated = true;
  }

  Tuple *forecast_bg_color_t = dict_find(iter, MESSAGE_KEY_PrefForecastBackgroundColor);
  if(forecast_bg_color_t) {
    settings.ForecastBackgroundColor = GColorFromHEX(forecast_bg_color_t->value->int32);
    settings_updated = true;
  }

  Tuple *date_strftime_t = dict_find(iter, MESSAGE_KEY_PrefDateStrftimeStr);
  if(date_strftime_t) {
    strncpy(settings.DateStrftimeStr, date_strftime_t->value->cstring, sizeof(settings.DateStrftimeStr));
    settings_updated = true;
  }
  if (settings_updated) {
    apply_settings();
    fetch_weather();
  }

  // weather icon
  for (idx = 0; idx < 3; idx++) {
    Tuple *response_weather_icon_t = dict_find(iter, MESSAGE_KEY_ResponseWeatherIcons + idx);
    if(response_weather_icon_t) {
      if (idx == 0) {
        s_weather_state.timestamp = time(NULL);
        s_fetched_initial_weather = true;
      }
      s_weather_state.weather_icon_id[idx] = response_weather_icon_t->value->int32;
      weather_updated = true;
    }
  }

  // weather time
  for (idx = 0; idx < 3; idx++) {
    Tuple *response_weather_time_str_t = dict_find(iter, MESSAGE_KEY_ResponseWeatherTimeStrs + idx);
    if(response_weather_time_str_t) {
      if (idx == 0) {
        s_weather_state.weather_time_buffer[idx][0] = 'n';
        s_weather_state.weather_time_buffer[idx][1] = 'o';
        s_weather_state.weather_time_buffer[idx][2] = 'w';
        s_weather_state.weather_time_buffer[idx][3] = '\0';
      } else {
        if (clock_is_24h_style()) {
          snprintf(s_weather_state.weather_time_buffer[idx], 5,
                   "%02d00",
                   (int) response_weather_time_str_t->value->int32);
        } else {
          if (response_weather_time_str_t->value->int32 > 12) {
            snprintf(s_weather_state.weather_time_buffer[idx], 5,
                     "%dpm",
                     (int) response_weather_time_str_t->value->int32 - 12);
          } else {
            snprintf(s_weather_state.weather_time_buffer[idx], 5,
                     "%dam",
                     response_weather_time_str_t->value->int32 == 0 ?
                     12 :
                     (int) response_weather_time_str_t->value->int32);
          }
        }
      }
      weather_updated = true;
    }
  }

  // weather temp
  for (idx = 0; idx < 3; idx++) {
    Tuple *response_weather_temp_str_t = dict_find(iter, MESSAGE_KEY_ResponseWeatherTempStrs + idx);
    if(response_weather_temp_str_t) {
      strncpy(s_weather_state.weather_temp_buffer[idx], response_weather_temp_str_t->value->cstring, sizeof(s_weather_state.weather_temp_buffer[idx]));
      weather_updated = true;
    }
  }

  if (weather_updated) {
    display_weather_state();
  }
}


static void bluetooth_connection_callback(bool connected) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "bluetooth connection event %d", connected);
  layer_set_hidden((Layer *)s_bt_icon_layer, connected);

  layer_mark_dirty((Layer *)s_bt_icon_layer);
}

static void prv_init(void) {
  // settings_init()
  settings.TimeFontColor = GColorWhite;
  settings.DateFontColor = GColorLightGray;
  settings.WeatherFontColor = GColorBlack;
  settings.TimeDateBackgroundColor = GColorDukeBlue;
  settings.ForecastBackgroundColor = GColorVividCerulean;
  strncpy(settings.DateStrftimeStr, "%a %d%b", sizeof(settings.DateStrftimeStr));
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
  s_weather_state.timestamp = 0;

  persist_read_data(WEATHER_KEY, &s_weather_state, sizeof(s_weather_state));
  if (timestamp_age(s_weather_state.timestamp) > 45 * SECONDS_PER_MINUTE) {
    set_default_weather_state();
  }

  // message_init() AppMessage connection
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(256, 128);

  // time and date initial values
  update_time_str(now());
  update_date_str(now());

  // fonts
  leco_14 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_LECO_FONT_14));
  leco_16 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_LECO_FONT_16));
  leco_25 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_LECO_FONT_25));
  leco_53 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_LECO_FONT_53));

  // bt icons
  bt_icon_disconnected = gbitmap_create_with_resource(RESOURCE_ID_BT_ICON_DISCONNECTED);

  // weather icons
  weather_chanceflurries_bitmap    = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_chanceflurries);
  weather_chancerain_bitmap        = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_chancerain);
  weather_chancesleet_bitmap       = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_chancesleet);
  weather_chancesnow_bitmap        = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_chancesnow);
  weather_chancetstorms_bitmap     = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_chancetstorms);
  weather_clear_bitmap             = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_clear);
  weather_cloudy_bitmap            = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_cloudy);
  weather_flurries_bitmap          = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_flurries);
  weather_fog_bitmap               = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_fog);
  weather_hazy_bitmap              = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_hazy);
  weather_mostlycloudy_bitmap      = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_mostlycloudy);
  weather_mostlysunny_bitmap       = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_mostlysunny);
  weather_nt_chanceflurries_bitmap = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_nt_chanceflurries);
  weather_nt_chancerain_bitmap     = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_nt_chancerain);
  weather_nt_chancesleet_bitmap    = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_nt_chancesleet);
  weather_nt_chancesnow_bitmap     = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_nt_chancesnow);
  weather_nt_chancetstorms_bitmap  = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_nt_chancetstorms);
  weather_nt_clear_bitmap          = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_nt_clear);
  weather_nt_cloudy_bitmap         = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_nt_cloudy);
  weather_nt_flurries_bitmap       = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_nt_flurries);
  weather_nt_fog_bitmap            = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_nt_fog);
  weather_nt_hazy_bitmap           = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_nt_hazy);
  weather_nt_mostlycloudy_bitmap   = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_nt_mostlycloudy);
  weather_nt_mostlysunny_bitmap    = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_nt_mostlysunny);
  weather_nt_partlycloudy_bitmap   = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_nt_partlycloudy);
  weather_nt_partlysunny_bitmap    = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_nt_partlysunny);
  weather_nt_rain_bitmap           = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_nt_rain);
  weather_nt_sleet_bitmap          = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_nt_sleet);
  weather_nt_snow_bitmap           = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_nt_snow);
  weather_nt_sunny_bitmap          = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_nt_sunny);
  weather_nt_tstorms_bitmap        = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_nt_tstorms);
  weather_partlycloudy_bitmap      = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_partlycloudy);
  weather_partlysunny_bitmap       = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_partlysunny);
  weather_rain_bitmap              = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_rain);
  weather_sleet_bitmap             = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_sleet);
  weather_snow_bitmap              = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_snow);
  weather_sunny_bitmap             = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_sunny);
  weather_tstorms_bitmap           = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_tstorms);
  weather_unknown_bitmap           = gbitmap_create_with_resource(RESOURCE_ID_WEATHER_ICON_unknown);

  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
      .load = prv_window_load,
        .unload = prv_window_unload,
        });
  const bool animated = true;
  window_stack_push(s_window, animated);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  //tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  apply_settings();
  bluetooth_connection_callback(connection_service_peek_pebble_app_connection());
  s_bt_handle = events_connection_service_subscribe((ConnectionHandlers) {
      .pebble_app_connection_handler = bluetooth_connection_callback
        });
  display_weather_state();
}

// Save the settings to persistent storage
static void save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  persist_write_data(WEATHER_KEY, &s_weather_state, sizeof(s_weather_state));
}

static void prv_deinit(void) {
  save_settings();
  events_connection_service_unsubscribe(s_bt_handle);
  window_destroy(s_window);
}

int main(void) {
  prv_init();

  // APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);

  app_event_loop();
  prv_deinit();
}



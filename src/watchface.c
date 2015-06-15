#include <pebble.h>
#include <string.h>

static Window *window;
static TextLayer *time_layer;
static ScrollLayer *scroll_layer;
static TextLayer *cheatsheet_layer;
static bool showTime = true;
static char cheatsheet_text[] = "Shell Built-in Variables:\n"
                                "------------------------\n"
                                "$0 #Name of this shell script itself.\n"
                                "$1 #Value of first command line parameter (similarly $2, $3, etc)\n"
                                "$# #In a shell script, the number of command line parameters.\n"
                                "$* #All of the command line parameters.\n"
                                "$- #Options given to the shell.\n"
                                "$? #Return the exit status of the last command.\n"
                                "$$ #Process id of script (really id of the shell running the script)\n";

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  static char buffer[] = "00:00";
  strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  text_layer_set_text(time_layer, buffer);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  layer_set_hidden((Layer *) time_layer, showTime);
  layer_set_hidden((Layer *) cheatsheet_layer, !showTime);
  showTime = !showTime;
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (!showTime) {
    GPoint offset = scroll_layer_get_content_offset(scroll_layer);
    offset.y = offset.y + 23;
    scroll_layer_set_content_offset(scroll_layer, offset, false);
  }
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (!showTime) {
    GPoint offset = scroll_layer_get_content_offset(scroll_layer);
    offset.y = offset.y - 23;
    scroll_layer_set_content_offset(scroll_layer, offset, false);
  }
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  
  time_layer = text_layer_create(GRect(0, 50, 144, 55));
  text_layer_set_background_color(time_layer, GColorWhite);
  text_layer_set_text_color(time_layer, GColorBlack);
  text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, 
                  text_layer_get_layer(time_layer));
  
  GRect bounds = layer_get_bounds(window_layer);
  const GRect max_text_bounds = GRect(0, 0, bounds.size.w, 20000);
  scroll_layer = scroll_layer_create(bounds);
  scroll_layer_set_content_size(scroll_layer, max_text_bounds.size);
  cheatsheet_layer = text_layer_create(max_text_bounds);
  text_layer_set_font(cheatsheet_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  scroll_layer_add_child(scroll_layer, text_layer_get_layer(cheatsheet_layer));
  layer_add_child(window_layer, scroll_layer_get_layer(scroll_layer));
  layer_set_hidden((Layer *) cheatsheet_layer, true);
  window_set_click_config_provider(window, (ClickConfigProvider) click_config_provider);
  text_layer_set_text(cheatsheet_layer, cheatsheet_text);
}

static void window_unload(Window *window) {
  text_layer_destroy(time_layer);
  scroll_layer_destroy(scroll_layer);
  text_layer_destroy(cheatsheet_layer);
}
  
static void init() {
  window = window_create();
  window_set_fullscreen(window, true);
  window_set_window_handlers(window, (WindowHandlers) {
    .load=window_load,
    .unload=window_unload,
  });
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  const bool animated = true;
  window_stack_push(window, animated);
}
  
int main() {
  init();
  app_event_loop();
  window_destroy(window);
}
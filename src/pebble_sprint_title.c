#include "pebble_sprint_title.h"

// UI
static Window *s_window;
static Layer *s_canvas;
//static TextLayer *s_title_layer, *s_up_layer, *s_select_layer, *s_down_layer;
//static BitmapLayer *s_bg_layer;
//static GBitmap *s_bg_bitmap;
static GBitmap *s_car_line;
static GBitmap *s_single_race;
static GBitmap *s_pebble_sprint;
static bool s_title_loaded = false;

static PGEClickHandler *s_click_handler;

// State
static int s_background_res_id;
static GColor s_title_color;
//static char s_title_buffer[TITLE_LENGTH_MAX];
//static char s_up_buffer[TITLE_ACTION_MAX];
//static char s_select_buffer[TITLE_ACTION_MAX];
//static char s_down_buffer[TITLE_ACTION_MAX];

/*********************************** Clicks ***********************************/

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_click_handler(BUTTON_ID_UP, false);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_click_handler(BUTTON_ID_SELECT, false);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_click_handler(BUTTON_ID_DOWN, false);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void draw_title(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorJaegerGreen);
  graphics_fill_rect(ctx, layer_get_frame(s_canvas), 0, GCornerNone);

  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  GRect bounds = gbitmap_get_bounds(s_car_line);
  graphics_draw_bitmap_in_rect(ctx, s_car_line, GRect(1, 5, bounds.size.w, bounds.size.h));

  bounds = gbitmap_get_bounds(s_single_race);
  graphics_draw_bitmap_in_rect(ctx, s_single_race, GRect(65, 10, bounds.size.w, bounds.size.h));

  bounds = gbitmap_get_bounds(s_pebble_sprint);
  graphics_draw_bitmap_in_rect(ctx, s_pebble_sprint, GRect(22, 60, bounds.size.w, bounds.size.h));
}

/********************************* Window *************************************/

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  // Set up canvas
  s_canvas = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  layer_set_update_proc(s_canvas, draw_title);
  layer_add_child(window_layer, s_canvas);

  s_car_line = gbitmap_create_with_resource(RESOURCE_ID_CAR_LINE);
  s_single_race = gbitmap_create_with_resource(RESOURCE_ID_SINGLE_RACE);
  s_pebble_sprint = gbitmap_create_with_resource(RESOURCE_ID_PEBBLE_SPRINT);

  // Allocate background
  //if(s_bg_bitmap) {
  //  gbitmap_destroy(s_bg_bitmap);
  //}
  //s_bg_bitmap = gbitmap_create_with_resource(s_background_res_id);

  // BG Layer
  //s_bg_layer = bitmap_layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  //bitmap_layer_set_bitmap(s_bg_layer, s_bg_bitmap);
  //layer_add_child(window_layer, bitmap_layer_get_layer(s_bg_layer));
#if 0
  // Title
  s_title_layer = text_layer_create(GRect(10, 40, window_bounds.size.w - 20, 60));
  text_layer_set_text_color(s_title_layer, s_title_color);
  text_layer_set_background_color(s_title_layer, GColorClear);
  text_layer_set_text_alignment(s_title_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_title_layer, GTextOverflowModeWordWrap);
  text_layer_set_font(s_title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text(s_title_layer, s_title_buffer);
  layer_add_child(window_layer, text_layer_get_layer(s_title_layer));

  // UP TextLayer
  s_up_layer = text_layer_create(GRect(0, 20, window_bounds.size.w, 30));
  text_layer_set_text_color(s_up_layer, s_title_color);
  text_layer_set_background_color(s_up_layer, GColorClear);
  text_layer_set_text_alignment(s_up_layer, GTextAlignmentRight);
  text_layer_set_font(s_up_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text(s_up_layer, s_up_buffer);
  layer_add_child(window_layer, text_layer_get_layer(s_up_layer));

  // SELECT TextLayer
  s_select_layer = text_layer_create(GRect(0, 75, window_bounds.size.w, 30));
  text_layer_set_text_color(s_select_layer, s_title_color);
  text_layer_set_background_color(s_select_layer, GColorClear);
  text_layer_set_text_alignment(s_select_layer, GTextAlignmentRight);
  text_layer_set_font(s_select_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text(s_select_layer, s_select_buffer);
  layer_add_child(window_layer, text_layer_get_layer(s_select_layer));

  // DOWN TextLayer
  s_down_layer = text_layer_create(GRect(0, 130, window_bounds.size.w, 30));
  text_layer_set_text_color(s_down_layer, s_title_color);
  text_layer_set_background_color(s_down_layer, GColorClear);
  text_layer_set_text_alignment(s_down_layer, GTextAlignmentRight);
  text_layer_set_font(s_down_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text(s_down_layer, s_down_buffer);
  layer_add_child(window_layer, text_layer_get_layer(s_down_layer));
#endif
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Loaded Title Screen");
}

static void window_unload(Window *window) {
  layer_destroy(s_canvas);
  s_canvas = NULL;

  gbitmap_destroy(s_car_line);
  gbitmap_destroy(s_single_race);
  gbitmap_destroy(s_pebble_sprint);
  s_car_line = NULL;
  s_single_race = NULL;
  s_pebble_sprint = NULL;
  //gbitmap_destroy(s_bg_bitmap);
  //bitmap_layer_destroy(s_bg_layer);
  //s_bg_bitmap = NULL;
  //s_bg_layer = NULL;

#if 0
  text_layer_destroy(s_title_layer);
  text_layer_destroy(s_up_layer);
  text_layer_destroy(s_select_layer);
  text_layer_destroy(s_down_layer);
#endif

  // Finally
  window_destroy(window);
  s_window = NULL;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Unloaded Title Screen");
}

/********************************* Public *************************************/

void title_push(int background_res_id, PGEClickHandler *click_handler) {
  if (s_title_loaded) {
    return;
  }
  // Store values
  s_title_color = GColorBlack;
  s_background_res_id = background_res_id;
#if 0
  snprintf(s_title_buffer, sizeof(s_title_buffer), "%s", "Pebble Sprint");
  snprintf(s_up_buffer, sizeof(s_up_buffer), "%s", "Single Race");
  snprintf(s_select_buffer, sizeof(s_select_buffer), "%s", "Tournament");
  snprintf(s_down_buffer, sizeof(s_down_buffer), "%s", "Settings");
#endif

  s_click_handler = click_handler;

  // Create Window
  if(!s_window) {
    s_window = window_create();
    window_set_click_config_provider(s_window, click_config_provider);
    window_set_fullscreen(s_window, true);
    window_set_window_handlers(s_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload
    });
  }
  window_stack_push(s_window, true);
  s_title_loaded = true;
}

void title_pop() {
  if (s_title_loaded) {
    // Should self-destroy
    window_stack_pop(true);
    s_window = NULL;
  }
  s_title_loaded = false;
}


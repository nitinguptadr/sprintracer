#include "sprintracer_title.h"
#include "sprintracer_common.h"

// UI
static Window *s_window;
static Layer *s_canvas;
static GBitmap *s_car_line;
static GBitmap *s_single_race;
static GBitmap *s_sprintracer;
static GBitmap *s_settings;
static GBitmap *s_tournament;
static bool s_title_loaded = false;
static PGEClickHandler *s_click_handler;

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

  bounds = gbitmap_get_bounds(s_sprintracer);
  graphics_draw_bitmap_in_rect(ctx, s_sprintracer, GRect(22, 60, bounds.size.w, bounds.size.h));

  bounds = gbitmap_get_bounds(s_settings);
  graphics_draw_bitmap_in_rect(ctx, s_settings, GRect(65, 142, bounds.size.w, bounds.size.h));

  bounds = gbitmap_get_bounds(s_tournament);
  graphics_draw_bitmap_in_rect(ctx, s_tournament, GRect(125, 45, bounds.size.w, bounds.size.h));
}

/********************************* Window *************************************/
static void window_appear(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Appearing Title Screen... %p", window);

  game_deinit();

  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  // Set up canvas
  s_canvas = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  if (!s_canvas) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Could not allocate s_canvas");
  }
  layer_set_update_proc(s_canvas, draw_title);
  layer_add_child(window_layer, s_canvas);

  s_car_line = gbitmap_create_with_resource(RESOURCE_ID_CAR_LINE);
  if (!s_car_line) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Could not allocate s_car_line");
  }

  s_single_race = gbitmap_create_with_resource(RESOURCE_ID_SINGLE_RACE);
  if (!s_single_race) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Could not allocate s_single_race");
  }

  s_sprintracer = gbitmap_create_with_resource(RESOURCE_ID_PEBBLE_SPRINT);
  if (!s_sprintracer) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Could not allocate s_sprintracer");
  }

  s_settings = gbitmap_create_with_resource(RESOURCE_ID_SETTINGS);
  if (!s_settings) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Could not allocate s_settings");
  }

  s_tournament = gbitmap_create_with_resource(RESOURCE_ID_TOURNAMENT);
  if (!s_tournament) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Could not allocate s_tournament");
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Appear Title Screen %p", window);
}

static void window_disappear(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Disappearing Title Screen... %p", window);

  layer_remove_from_parent(s_canvas);
  layer_destroy(s_canvas);
  s_canvas = NULL;

  if (s_car_line) {
    gbitmap_destroy(s_car_line);
    s_car_line = NULL;
  }
  if (s_single_race) {
    gbitmap_destroy(s_single_race);
    s_single_race = NULL;
  }
  if (s_sprintracer) {
    gbitmap_destroy(s_sprintracer);
    s_sprintracer = NULL;
  }
  if (s_settings) {
    gbitmap_destroy(s_settings);
    s_settings = NULL;
  }
  if (s_tournament) {
    gbitmap_destroy(s_tournament);
    s_tournament = NULL;
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Disappear Title Screen %p", window);
}

static void window_unload(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Unloading Title Screen %p", window);
  // Finally
  window_destroy(window);
  s_window = NULL;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Unloaded Title Screen %p", window);
}

/********************************* Public *************************************/

void title_push(PGEClickHandler *click_handler) {
  if (s_title_loaded) {
    return;
  }
  s_click_handler = click_handler;

  // Create Window
  if(!s_window) {
    s_window = window_create();
    window_set_click_config_provider(s_window, click_config_provider);
#ifdef PBL_PLATFORM_APLITE
    window_set_fullscreen(s_window, true);
#endif
    window_set_window_handlers(s_window, (WindowHandlers) {
      .unload = window_unload,
      .appear = window_appear,
      .disappear = window_disappear
    });
  }
  window_stack_push(s_window, true);
  s_title_loaded = true;
}

void title_pop() {
  if (s_title_loaded) {
    // Should self-destroy
    window_stack_pop(true);
  }
  s_title_loaded = false;
}


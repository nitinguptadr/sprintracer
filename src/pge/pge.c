#include "pge.h"

// State
static Window *s_game_window;
static Layer *s_canvas;
static AppTimer *s_render_timer;
static BitmapLayer *s_bg_layer;
static GBitmap *s_bg_bitmap;

static PGELogicHandler *s_logic_handler;
static PGERenderHandler *s_render_handler;
static PGEClickHandler *s_click_handler;

static bool s_button_states[3];
static int s_framerate;

// Internal prototypes
static void game_window_load(Window *window);
static void game_window_unload(Window *window);
static void frame_timer_handler(void *context);
static void draw_frame_update_proc(Layer *layer, GContext *ctx);
static void click_config_provider(void *context);

/*********************************** Engine ***********************************/

/**
 * pge_init() and pge_deinit() should be implemented in app's main C file
 */
int main(void) {
  pge_init();
  app_event_loop();
  pge_deinit();
}

Window* pge_begin(GColor window_color, PGELogicHandler *logic_handler, PGERenderHandler *render_handler, PGEClickHandler *click_handler) {
  s_logic_handler = logic_handler;
  s_render_handler = render_handler;

  s_game_window = window_create();
#ifdef PBL_PLATFORM_APLITE
  window_set_fullscreen(s_game_window, true);
#endif
  window_set_background_color(s_game_window, window_color);
  window_set_window_handlers(s_game_window, (WindowHandlers) {
    .load = game_window_load,
    .unload = game_window_unload
  });

  if(click_handler != NULL) {
    s_click_handler = click_handler;
    window_set_click_config_provider(s_game_window, click_config_provider);
  }

  // Go!
  window_stack_push(s_game_window, true);
  return s_game_window;
}

void pge_finish() {
  // Stop the game
  if(s_render_timer != NULL) {
    // Cancel any Timer
    app_timer_cancel(s_render_timer);
    s_render_timer = NULL;
  }

  // Finally
  window_destroy(s_game_window);
  s_game_window = NULL;
}

bool pge_get_button_state(ButtonId button) {
  switch(button) {
    case BUTTON_ID_UP:
      return s_button_states[0];
    case BUTTON_ID_SELECT:
      return s_button_states[1];
    case BUTTON_ID_DOWN:
      return s_button_states[2];
    default: 
      return false;
  }
}

void pge_set_framerate(int new_rate) {
  s_framerate = new_rate;
}

void pge_set_background(int bg_resource_id) {
  if(s_bg_bitmap) {
    gbitmap_destroy(s_bg_bitmap);
  }
  s_bg_bitmap = gbitmap_create_with_resource(bg_resource_id);
  bitmap_layer_set_bitmap(s_bg_layer, s_bg_bitmap);
}

void pge_manual_advance() {
  layer_mark_dirty(s_canvas);
}

Layer *pge_get_canvas() {
  return s_canvas;
}

/************************* Engine Internal Functions **************************/

static void game_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  // Set up background
  s_bg_layer = bitmap_layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bg_layer));

  // Set up canvas
  s_canvas = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  layer_set_update_proc(s_canvas, draw_frame_update_proc);
  layer_add_child(window_layer, s_canvas);

  // Register new Timer to begin frame rendering loop
  s_render_timer = app_timer_register(1000 / s_framerate, frame_timer_handler, NULL);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Game window loaded %p", window);
}

static void game_window_unload(Window *window) {
  // Destroy canvas
  layer_destroy(s_canvas);
  bitmap_layer_destroy(s_bg_layer);
  gbitmap_destroy(s_bg_bitmap);
  s_bg_bitmap = NULL;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Game window unloaded %p", window);
}

static void frame_timer_handler(void *context) {
  if(s_logic_handler != NULL && s_render_handler != NULL) {
    // Do this frame
    layer_mark_dirty(s_canvas);

    // Next frame
    s_render_timer = app_timer_register(1000 / s_framerate, frame_timer_handler, NULL);
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Loop or Render handler not set!");
  }
}

static void draw_frame_update_proc(Layer *layer, GContext *ctx) {
  // Render and logic
  if(s_logic_handler != NULL && s_render_handler != NULL) {
    s_render_handler(ctx);
    s_logic_handler();
  } else {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Loop or Render handler not set!");
  }
}

static void up_pressed_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_button_states[0] = true;
}

static void up_released_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_button_states[0] = false;
}

static void select_pressed_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_button_states[1] = true;
}

static void select_released_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_button_states[1] = false;
}

static void down_pressed_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_button_states[2] = true;
}

static void down_released_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_button_states[2] = false;
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_click_handler(BUTTON_ID_SELECT, false);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_click_handler(BUTTON_ID_UP, false);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_click_handler(BUTTON_ID_DOWN, false);
}

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_click_handler(BUTTON_ID_SELECT, true);
}

static void up_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_click_handler(BUTTON_ID_UP, true);
}

static void down_long_click_handler(ClickRecognizerRef recognizer, void *context) {
  s_click_handler(BUTTON_ID_DOWN, true);
}

static void click_config_provider(void *context) {
  window_raw_click_subscribe(BUTTON_ID_UP, up_pressed_click_handler, up_released_click_handler, NULL);
  window_raw_click_subscribe(BUTTON_ID_SELECT, select_pressed_click_handler, select_released_click_handler, NULL);
  window_raw_click_subscribe(BUTTON_ID_DOWN, down_pressed_click_handler, down_released_click_handler, NULL);

  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);

  window_long_click_subscribe(BUTTON_ID_SELECT, 500, select_long_click_handler, NULL);
  window_long_click_subscribe(BUTTON_ID_UP, 500, up_long_click_handler, NULL);
  window_long_click_subscribe(BUTTON_ID_DOWN, 500, down_long_click_handler, NULL);
}
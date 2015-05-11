#include "sprintracer_title.h"
#include "sprintracer_level.h"

// UI
static Window *s_window;
static TextLayer *s_level_selector_layer;
static GBitmap *s_level_bitmap;
static BitmapLayer *s_bitmap_layer;

// State
static char s_level_buffer[TITLE_LENGTH_MAX];

static void level_selector_window_pop();

static void update_level_image(LevelNumId current_level) {
  int resource_id = RESOURCE_ID_LEVEL_0;

  switch (current_level) {
    case LEVEL0_ID:
      resource_id = RESOURCE_ID_LEVEL_0;
      break;
    case LEVEL1_ID:
      resource_id = RESOURCE_ID_LEVEL_1;
      break;
    case LEVEL2_ID:
      resource_id = RESOURCE_ID_LEVEL_2;
      break;
    default:
      resource_id = RESOURCE_ID_LEVEL_0;
      break;
  }

  // Allocate Image
  if(s_level_bitmap) {
    gbitmap_destroy(s_level_bitmap);
  }
  s_level_bitmap = gbitmap_create_with_resource(resource_id);
  bitmap_layer_set_bitmap(s_bitmap_layer, s_level_bitmap);
}

/*********************************** Clicks ***********************************/

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  LevelNumId current_level = level_get_current();
  current_level = (current_level + 1) % LEVEL_COUNT;
  level_set_current(current_level);
  snprintf(s_level_buffer, sizeof(s_level_buffer), "Level %d", current_level);
  update_level_image(current_level);
  layer_mark_dirty(bitmap_layer_get_layer(s_bitmap_layer));
  layer_mark_dirty(text_layer_get_layer(s_level_selector_layer));
}

extern void car_selector_window_push();
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  level_selector_window_pop();
  car_selector_window_push();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  LevelNumId current_level = level_get_current();
  current_level = current_level == 0 ? LEVEL_COUNT - 1 : current_level - 1;
  level_set_current(current_level);
  snprintf(s_level_buffer, sizeof(s_level_buffer), "Level %d", current_level);
  update_level_image(current_level);
  layer_mark_dirty(bitmap_layer_get_layer(s_bitmap_layer));
  layer_mark_dirty(text_layer_get_layer(s_level_selector_layer));
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

/********************************* Window *************************************/

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  window_set_background_color(window, GColorJaegerGreen);

  LevelNumId current_level = level_get_current();

  // Level Image Layer
  s_bitmap_layer = bitmap_layer_create(GRect((SCREEN_RES_COLS - 100) / 2, 40, 100, 125));
  update_level_image(current_level);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));

  // Title
  snprintf(s_level_buffer, sizeof(s_level_buffer), "Level %d", current_level);
  s_level_selector_layer = text_layer_create(GRect(10, 5, window_bounds.size.w - 20, 30));
  text_layer_set_text_color(s_level_selector_layer, GColorBlack);
  text_layer_set_background_color(s_level_selector_layer, GColorClear);
  text_layer_set_text_alignment(s_level_selector_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(s_level_selector_layer, GTextOverflowModeWordWrap);
  text_layer_set_font(s_level_selector_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text(s_level_selector_layer, s_level_buffer);
  layer_add_child(window_layer, text_layer_get_layer(s_level_selector_layer));
}

static void window_unload(Window *window) {
  gbitmap_destroy(s_level_bitmap);
  s_level_bitmap = NULL;
  bitmap_layer_destroy(s_bitmap_layer);
  s_bitmap_layer = NULL;
  text_layer_destroy(s_level_selector_layer);
  s_level_selector_layer = NULL;

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Level %d Selected", level_get_current());
  // Finally
  window_destroy(window);
  s_window = NULL;
}

/********************************* Public *************************************/
void level_selector_window_push() {
  game_deinit();

  // Create Window
  if(!s_window) {
    s_window = window_create();
    window_set_click_config_provider(s_window, click_config_provider);
#ifdef PBL_PLATFORM_APLITE
    window_set_fullscreen(s_window, true);
#endif
    window_set_window_handlers(s_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload
    });
  }
  window_stack_push(s_window, true);
}

void level_selector_window_pop() {
  // Should self-destroy
  window_stack_pop(true);
  s_window = NULL;
}


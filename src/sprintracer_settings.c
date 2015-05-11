#include "sprintracer_title.h"
#include "sprintracer_level.h"
#include "pge/additional/pge_title.h"

// UI
static Window *s_window;
static MenuLayer *s_menu_layer;

static ButtonActionIds s_button_actions;
static bool s_button_actions_set = false;
static int s_button_actions_index = 0;

static void settings_window_pop();

#define NUM_MENU_SECTIONS     2
#define NUM_CONTROLS_SETTINGS 2
#define NUM_FIRST_MENU_ITEMS  NUM_CONTROLS_SETTINGS 
#define NUM_HIGH_SCORE_ITEMS 1

static GBitmap* s_controls_bitmaps[1];

void set_button_actions(int cw, int select, int ccw) {
  s_button_actions.clockwise = cw;
  s_button_actions.select = select;
  s_button_actions.counterclockwise = ccw;
  s_button_actions_set = true;
}

ButtonActionIds* get_button_actions() {
  return &s_button_actions;
}

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    case 0:
      return NUM_FIRST_MENU_ITEMS;
    case 1:
      return NUM_HIGH_SCORE_ITEMS;
    default:
      return 0;
  }
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  switch (section_index) {
    case 0:
      // Draw title text in the section header
      menu_cell_basic_header_draw(ctx, cell_layer, "CONTROLS");
      break;
    case 1: {
        // Draw title text in the section header
      char buff[20];
      snprintf(buff, sizeof(buff), "HIGH SCORE: %d", pge_title_get_highscore());
      menu_cell_basic_header_draw(ctx, cell_layer, buff);
      break;
    }
  }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item we'll draw
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  if (cell_index->section == 0) {
    switch (cell_index->row) {
      case 0:
        menu_cell_basic_draw(ctx, cell_layer, s_button_actions_index == 0 ? "Selected" : NULL, "UP=CCW, DOWN=CW", s_button_actions_index == 0 ? NULL /*s_controls_bitmaps[0]*/ : NULL);
        break;
      case 1:
        menu_cell_basic_draw(ctx, cell_layer, s_button_actions_index == 1 ? "Selected" : NULL, "UP=CW, DOWN=CCW", s_button_actions_index == 1 ? NULL /*s_controls_bitmaps[0]*/ : NULL);
        break;
    }
  } else if (cell_index->section == 1) {
    menu_cell_basic_draw(ctx, cell_layer, "Reset High Score?", "Press Select", NULL);
  }
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
  if (cell_index->section == 0) {
    switch (cell_index->row) {
      case 0:
        set_button_actions(BUTTON_ID_DOWN, BUTTON_ID_SELECT, BUTTON_ID_UP);
        s_button_actions_index = cell_index->row;
        break;
      case 1:
        set_button_actions(BUTTON_ID_UP, BUTTON_ID_SELECT, BUTTON_ID_DOWN);
        s_button_actions_index = cell_index->row;
        break;
    }
  } else if (cell_index->section == 1) {
    pge_title_set_highscore(0);
  }
  layer_mark_dirty(menu_layer_get_layer(menu_layer));
}

/********************************* Window *************************************/

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  window_set_background_color(window, GColorJaegerGreen);

  s_menu_layer = menu_layer_create(window_bounds);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });

//  for (int index = RESOURCE_ID_CONTROLS0; index < RESOURCE_ID_CONTROLS0 + NUM_FIRST_MENU_ITEMS; index++) {
//    s_controls_bitmaps[0] = gbitmap_create_with_resource(RESOURCE_ID_CONTROLS0);
//  }

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(s_menu_layer, window);

  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(s_menu_layer);

//  for (int index = RESOURCE_ID_CONTROLS0; index < RESOURCE_ID_CONTROLS0 + NUM_FIRST_MENU_ITEMS; index++) {
//    gbitmap_destroy(s_controls_bitmaps[0]);
//  }


  // Finally
  window_destroy(window);
  s_window = NULL;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Controls %d Selected ", s_button_actions_index);
}

/********************************* Public *************************************/
void settings_window_push() {
  game_deinit();

  if (!s_button_actions_set) {
    s_button_actions_index = 0;
    set_button_actions(BUTTON_ID_DOWN, BUTTON_ID_SELECT, BUTTON_ID_UP);
  }

  // Create Window
  if(!s_window) {
    s_window = window_create();
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

void settings_window_pop() {
  // Should self-destroy
  window_stack_pop(true);
  s_window = NULL;
}


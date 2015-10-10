#include "sprintracer_title.h"
#include "sprintracer_level.h"


#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ICONS NUM_CAR_RESOURCES
#define NUM_FIRST_MENU_ITEMS NUM_CAR_RESOURCES

// UI
static Window *s_window;
static MenuLayer *s_menu_layer;
static GBitmap *s_car_bitmaps[NUM_CAR_RESOURCES];

// Resource IDs for all cars;
static int current_cars[NUM_CARS_TOTAL];

static void car_selector_window_pop();

static void update_current_car(int index) {
  current_cars[0] = RESOURCE_ID_CAR_RED + index;
  current_cars[1] = current_cars[0] + 1;
  if (current_cars[1] >= RESOURCE_ID_CAR_RED + NUM_CAR_RESOURCES) {
    current_cars[1] = RESOURCE_ID_CAR_RED;
  }
  current_cars[2] = current_cars[1] + 1;
  if (current_cars[2] >= RESOURCE_ID_CAR_RED + NUM_CAR_RESOURCES) {
    current_cars[2] = RESOURCE_ID_CAR_RED;
  }
  current_cars[3] = current_cars[2] + 1;
  if (current_cars[3] >= RESOURCE_ID_CAR_RED + NUM_CAR_RESOURCES) {
    current_cars[3] = RESOURCE_ID_CAR_RED;
  }
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Car selections %d %d %d %d", current_cars[0], current_cars[1], current_cars[2], current_cars[3]);
}

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  switch (section_index) {
    default:
      return NUM_FIRST_MENU_ITEMS;
  }
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
#if defined(PBL_ROUND)
  return 3 * MENU_CELL_BASIC_HEADER_HEIGHT;
#else
  return MENU_CELL_BASIC_HEADER_HEIGHT;
#endif
}

static void prv_menu_cell_basic_header_draw(GContext* ctx, const Layer *cell_layer, const char *title) {
  // Title:
  if (title) {
    GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
    GRect box = layer_get_bounds(cell_layer);
    // Pixel nudging...
    box.origin.x += 2;
    box.origin.y += MENU_CELL_BASIC_HEADER_HEIGHT;
    graphics_draw_text(ctx, title, font, box, GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  }
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  switch (section_index) {
    default:
      // Draw title text in the section header
#if defined(PBL_ROUND)
      prv_menu_cell_basic_header_draw(ctx, cell_layer, "Select Your Car");
#else
      menu_cell_basic_header_draw(ctx, cell_layer, "Select Your Car");
#endif
      break;
  }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item we'll draw
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  switch (cell_index->row) {
    case 0:
      menu_cell_basic_draw(ctx, cell_layer, "Red", NULL, s_car_bitmaps[cell_index->row]);
      break;
    case 1:
      menu_cell_basic_draw(ctx, cell_layer, "Yellow", NULL, s_car_bitmaps[cell_index->row]);
      break;
    case 2:
      menu_cell_basic_draw(ctx, cell_layer, "Orange", NULL, s_car_bitmaps[cell_index->row]);
      break;
    case 3:
      menu_cell_basic_draw(ctx, cell_layer, "Green", NULL, s_car_bitmaps[cell_index->row]);
      break;
    case 4:
      menu_cell_basic_draw(ctx, cell_layer, "Blue", NULL, s_car_bitmaps[cell_index->row]);
      break;
    case 5:
      menu_cell_basic_draw(ctx, cell_layer, "Purple", NULL, s_car_bitmaps[cell_index->row]);
      break;
    case 6:
      menu_cell_basic_draw(ctx, cell_layer, "White", NULL, s_car_bitmaps[cell_index->row]);
      break;
  }
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  // Use the row to specify which item will receive the select action
  update_current_car(cell_index->row);
  car_selector_window_pop();
  game_init();
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

  menu_layer_set_normal_colors(s_menu_layer, GColorJaegerGreen, GColorWhite);
  menu_layer_set_highlight_colors(s_menu_layer, GColorBlack, GColorWhite);

  for (int index = RESOURCE_ID_CAR_RED; index < RESOURCE_ID_CAR_RED + NUM_MENU_ICONS; index++) {
    s_car_bitmaps[index - RESOURCE_ID_CAR_RED] = gbitmap_create_with_resource(index);
  }

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(s_menu_layer, window);

  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void window_unload(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Car %d Selected", current_cars[0]);

  // Destroy the menu layer
  menu_layer_destroy(s_menu_layer);

  for (int index = RESOURCE_ID_CAR_RED; index < RESOURCE_ID_CAR_RED + NUM_MENU_ICONS; index++) {
    gbitmap_destroy(s_car_bitmaps[index - RESOURCE_ID_CAR_RED]);
  }

  // Finally
  window_destroy(window);
  s_window = NULL;
}

/********************************* Public *************************************/
void car_selector_window_push() {
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

void car_selector_window_pop() {
  // Should self-destroy
  window_stack_pop(true);
  s_window = NULL;
}

int get_car_selection(int index) {
  if (index >= NUM_CARS_TOTAL) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "ERROR: get_car_selection - should not reach here");
    return 0;
  }
  return current_cars[index];
}

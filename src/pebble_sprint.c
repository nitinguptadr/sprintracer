#include <pebble.h>

#include <inttypes.h>
#include <math.h>

#include "pebble_sprint_common.h"
#include "pebble_sprint_level.h"

#include "pge/pge.h"
#include "pge/additional/pge_sprite.h"
#include "pge/additional/pge_title.h"
#include "pge/additional/pge_grid.h"
#include "pge/additional/pge_collision.h"
#include "pge/additional/pge_splash.h"

// Car dimensions
// 12px by 18 px - 0 degrees
// 18px by 18 px - 45 degrees

#define CAR_SIZE 18
#define CAR_SPEED_DEFAULT 3.5
#define CAR_SPEED_MAX 5
#define ANGLE_CHANGE 9
#define ANGLE_CHANGE_RESOURCE 45
#define ANGLE_MASK 360

#define DEG_TO_TRIG_ANGLE(angle) (((angle % 360) * TRIG_MAX_ANGLE) / 360)

static Window *s_game_window;
static TextLayer *s_game_info_layer;
#define TEXT_BUFFER_SIZE 20
static char s_game_info_buffer[TEXT_BUFFER_SIZE];
static bool s_game_initialized = false;
static GRect s_game_bounds;

static uint8_t count_down = 3;
static float car_speed = CAR_SPEED_DEFAULT;

static LevelNumId s_current_level = LEVEL_ID0;

enum {
  ROTATION_CCW = -1,  // counter-clockwise - UP button - add ANGLE_CHANGE
  ROTATION_NONE = 0,  // none - don't change
  ROTATION_CW = 1     // clockwise - DOWN button - subtract ANGLE_CHANGE
};
static int direction = ROTATION_NONE;

typedef struct {
  PGESprite *sprite_white;
  PGESprite *sprite_black;
  PGESprite *sprite_color;
  int resource_id;
  bool moving;
  int prev_angle;
  int angle;
  int32_t x_change;
  int32_t y_change;
  float pos_x;
  float pos_y;
} Car;

//static int s_direction, s_score;
//static bool s_moving = false;
PGESprite* bg_level = NULL;

Car* car_user = NULL;
Car* car_opp1 = NULL;
Car* car_opp2 = NULL;
Car* car_opp3 = NULL;

Car* car_pace = NULL;

Car* car_create(GPoint start_position, int angle, int resource_white, int resource_black, int resource_color) {
  Car *car_ptr = malloc(sizeof(Car));
  car_ptr->angle = angle;
  car_ptr->x_change = 0;
  car_ptr->y_change = 0;
  car_ptr->sprite_color = pge_sprite_create(start_position, resource_color);
#ifndef PBL_COLOR
  car_ptr->sprite_white = pge_sprite_create(start_position, resource_white);
  car_ptr->sprite_black = pge_sprite_create(start_position, resource_black);
#endif
  car_ptr->moving = false;
  car_ptr->resource_id = resource_white;
  return car_ptr;
}

void car_destroy(Car* car_ptr) {
  if (car_ptr) {
    pge_sprite_destroy(car_ptr->sprite_color);
#ifndef PBL_COLOR
  pge_sprite_destroy(car_ptr->sprite_white);
  pge_sprite_destroy(car_ptr->sprite_black); 
#endif
    free(car_ptr);
    car_ptr = NULL;
  }
}

void car_draw(Car* car_ptr, GContext *ctx) {
#ifdef PBL_COLOR
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  pge_sprite_draw(car_ptr->sprite_color, ctx);
#else
  graphics_context_set_compositing_mode(ctx, GCompOpClear);
  pge_sprite_draw(car_ptr->sprite_black, ctx);

  graphics_context_set_compositing_mode(ctx, GCompOpOr);
  pge_sprite_draw(car_ptr->sprite_white, ctx);
#endif
}

static void update_car_position(Car* car_ptr) {
  GPoint pos = pge_sprite_get_position(car_ptr->sprite_color);
//  snprintf(s_game_info_buffer, sizeof(s_game_info_buffer), "X:%d,Y%d", car_user->x_change, car_user->y_change);

  // Move square and wrap to screen
  if(car_ptr->moving) {

    // Check if car collides with any of the walls - use the center point of each of the four walls
    // of the sprite to check for the direction of the collision
    GRect car_bounds = pge_sprite_get_bounds(car_ptr->sprite_color);
    uint8_t allowable_directions = level_collision_walls(s_current_level, car_bounds);

    // Check if car collides with any other car
    allowable_directions &= level_collision_cars(car_bounds, pge_sprite_get_bounds(car_opp1->sprite_color));
    allowable_directions &= level_collision_cars(car_bounds, pge_sprite_get_bounds(car_opp2->sprite_color));
    allowable_directions &= level_collision_cars(car_bounds, pge_sprite_get_bounds(car_opp3->sprite_color));

    // Determine direction of moving car and based on allowable directions, update the car's
    // new position
    int32_t angle = DEG_TO_TRIG_ANGLE(car_ptr->angle);
    float sin_value = ((float)sin_lookup(angle)) / TRIG_MAX_RATIO;
    float cos_value = ((float)cos_lookup(angle)) / TRIG_MAX_RATIO;
    float x_direction = car_speed*sin_value;
    float y_direction = -car_speed*cos_value;

    // For   x direction - LEFT is negative X
    if (((x_direction > 0) && (allowable_directions & DIRECTION_RIGHT)) || 
        ((x_direction < 0) && (allowable_directions & DIRECTION_LEFT))) {
      car_ptr->pos_x += x_direction;
    }
    // For y direction - UP is negative Y
    if (((y_direction < 0) && (allowable_directions & DIRECTION_UP)) || 
        ((y_direction > 0) && (allowable_directions & DIRECTION_DOWN))) {
      car_ptr->pos_y += y_direction;
    }

    pos.x = (int16_t)car_ptr->pos_x;
    pos.y = (int16_t)car_ptr->pos_y;
    
    // Update sprite
    pge_sprite_set_position(car_ptr->sprite_color, pos);
#ifndef PBL_COLOR
    pge_sprite_set_position(car_ptr->sprite_black, pos);
    pge_sprite_set_position(car_ptr->sprite_white, pos);
#endif
  }
}

static void update_car_angle(Car* car_ptr, int change) {
  car_ptr->prev_angle = car_ptr->angle;
  if (change < 0) {
    car_ptr->angle = (car_ptr->angle + change + ANGLE_MASK) % ANGLE_MASK;
  } else {
    car_ptr->angle = (car_ptr->angle + change) % ANGLE_MASK;
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, "update_car_angle %d", car_ptr->angle);
  pge_sprite_set_rotation(car_ptr->sprite_color, DEG_TO_TRIG_ANGLE(car_ptr->angle));
}

void update_game_bounds() {
  // Update game frame based on the car position such that the car is centered in the frame
  Layer *game_layer = pge_get_canvas();
  GRect car_bounds = pge_sprite_get_bounds(car_user->sprite_color);
  s_game_bounds = GRect(-(car_bounds.origin.x - (SCREEN_RES_COLS / 2)), -(car_bounds.origin.y - (SCREEN_RES_ROWS / 2)),
                            SCREEN_RES_COLS, SCREEN_RES_ROWS);

  layer_set_bounds(game_layer, s_game_bounds);
}

static void logic() {
  //snprintf(s_game_info_buffer, sizeof(s_game_info_buffer), "speed:%ld", (int32_t)(car_speed*10));
  text_layer_set_text(s_game_info_layer, s_game_info_buffer);

  update_car_position(car_user);
  update_car_position(car_opp1);
  //update_car_position(car_opp2);
  //update_car_position(car_opp3);

  if (pge_get_button_state(BUTTON_ID_UP)) {
    direction = ROTATION_CCW;
    update_car_angle(car_user, direction * ANGLE_CHANGE); 
  } else if (pge_get_button_state(BUTTON_ID_DOWN)) {
    direction = ROTATION_CW;
    update_car_angle(car_user, direction * ANGLE_CHANGE); 
  } else {
    direction = ROTATION_NONE;
  }

  update_game_bounds();
}

void draw(GContext *ctx) {
  // Clear board
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(0, 0, SCREEN_RES_COLS, SCREEN_RES_ROWS), 0, GCornerNone);

  level_draw(ctx, s_game_bounds);

  car_draw(car_user, ctx);
  car_draw(car_opp1, ctx);
  car_draw(car_opp2, ctx);
  car_draw(car_opp3, ctx);
  car_draw(car_pace, ctx);
}

static void click(int button_id, bool long_click) {
  switch(button_id) {
    case BUTTON_ID_UP:
      direction = ROTATION_CCW;
      update_car_angle(car_user, direction * ANGLE_CHANGE); 
      break;

    case BUTTON_ID_SELECT:
      // TODO: Make this dependent on how many speed_ups are obtained
      car_speed = (car_speed + 0.5);
      if (car_speed > CAR_SPEED_MAX) {
        car_speed = 0.5;
      }
      car_user->moving = true;
      break;

    case BUTTON_ID_DOWN:
      direction = ROTATION_CW;
      update_car_angle(car_user, direction * ANGLE_CHANGE); 
      break;
  }
}

/******************************** Title ***************************************/

static void game_deinit() {
  text_layer_destroy(s_game_info_layer);
  level_deinitialize();

  car_destroy(car_user);
  car_user = NULL;
  car_destroy(car_opp1);
  car_opp1 = NULL;
  car_destroy(car_opp2);
  car_opp2 = NULL;
  car_destroy(car_opp3);
  car_opp3 = NULL;
  car_destroy(car_pace);
  car_pace = NULL;

  s_game_initialized = false;
}

static void game_init() {
  if (s_game_initialized) {
    game_deinit();
  }

  Layer *game_layer = pge_get_canvas();

  // Initialize Level
  s_current_level = LEVEL_ID0;
  level_initialize(game_layer, s_current_level);

  s_game_info_layer = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { 144, 20 } });
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Created text layer");
  //layer_add_child(game_layer, text_layer_get_layer(s_game_info_layer));
  text_layer_set_text(s_game_info_layer, s_game_info_buffer);
  text_layer_set_text_alignment(s_game_info_layer, GTextAlignmentCenter);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Printing buffer");
  //snprintf(s_game_info_buffer, sizeof(s_game_info_buffer), "X:%ld,Y%ld", car_user->x_change, car_user->y_change);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Creating Cars");

  car_user = car_create((GPoint){.x = 120, .y = 60}, 0,
                        RESOURCE_ID_CAR_RED, RESOURCE_ID_CAR_RED, RESOURCE_ID_CAR_RED);

  car_opp1 = car_create((GPoint){.x = 145, .y = 60}, 0,
                        RESOURCE_ID_CAR_BLUE, RESOURCE_ID_CAR_BLUE, RESOURCE_ID_CAR_BLUE);

  car_opp2 = car_create((GPoint){.x = 120, .y = 82}, 0,
                        RESOURCE_ID_CAR_ORANGE, RESOURCE_ID_CAR_ORANGE, RESOURCE_ID_CAR_ORANGE);

  car_opp3 = car_create((GPoint){.x = 145, .y = 82}, 0,
                        RESOURCE_ID_CAR_PURPLE, RESOURCE_ID_CAR_PURPLE, RESOURCE_ID_CAR_PURPLE);

  car_pace = car_create((GPoint){.x = 120, .y = 34}, 0,
                        RESOURCE_ID_CAR_WHITE, RESOURCE_ID_CAR_WHITE, RESOURCE_ID_CAR_WHITE);

  update_car_angle(car_user, -90);
  update_car_angle(car_opp1, -90);
  update_car_angle(car_opp2, -90);
  update_car_angle(car_opp3, -90);

  GPoint pos = pge_sprite_get_position(car_user->sprite_color);
  car_user->pos_x = pos.x;
  car_user->pos_y = pos.y;


  APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating bounds");
  update_game_bounds();

  s_game_initialized = true;

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Game initialized");

}

static void title_click(int button_id, bool long_click) {
  switch(button_id) {
    // Some other action
    case BUTTON_ID_SELECT:
      car_speed = (car_speed + 0.5);
      if (car_speed > CAR_SPEED_MAX) {
        car_speed = 0.5;
      }
      break; 

    case BUTTON_ID_DOWN:
      game_init();
      if (car_speed == 0) {
        car_speed = CAR_SPEED_DEFAULT;
      }
      // Go to game!
      window_stack_push(s_game_window, true);
      break;
  }
}

/******************************** App *****************************************/

static void pebble_sprint_init(void) {
  s_game_window = pge_begin(GColorBlack, logic, draw, click);
  pge_set_framerate(20);

  pge_title_push("Pebble\nSprint", "SPEED >", "PLAY >", GColorBlack, 0, title_click);

  //pge_set_background(RESOURCE_ID_BG_LEVEL_0);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Main Window Loaded--");
}

void pge_init() {
  srand(time(NULL));
  car_speed = CAR_SPEED_DEFAULT;

  s_game_initialized = false;
  pebble_sprint_init();
  //pge_splash_show(pebble_sprint_init);
}

void pge_deinit() {
  game_deinit();

  // Destroy all game resources
  pge_finish();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Main Window UnLoaded");
}


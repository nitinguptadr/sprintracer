#include <pebble.h>

#include <inttypes.h>
#include <math.h>

#include "pebble_sprint_common.h"
#include "pebble_sprint_level.h"
#include "pebble_sprint_title.h"

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
#define ANGLE_CHANGE_RESOURCE 45
#define ANGLE_MASK 360
static float car_speed = CAR_SPEED_DEFAULT;

static Window *s_game_window;
static TextLayer *s_game_info_layer;
#define TEXT_BUFFER_SIZE 20
static char s_game_info_buffer[TEXT_BUFFER_SIZE];
static bool s_game_initialized = false;
static GRect s_game_bounds;

static bool s_title_pushed = false;


#ifdef DEBUG
#define NUM_LAPS_DEFAULT 1
#else
#define NUM_LAPS_DEFAULT 3
#endif

static unsigned int s_num_laps = NUM_LAPS_DEFAULT;

#define STATUS_LAYER_HEIGHT 16
#define STATUS_LAYER_RECT GRect(0, SCREEN_RES_ROWS - STATUS_LAYER_HEIGHT, SCREEN_RES_COLS, STATUS_LAYER_HEIGHT)
static TextLayer *s_status_layer;
static char s_status_text[20];

#define COUNTDOWN_MAX -1
static int s_countdown = 3;
static AppTimer *s_countdown_timer;

static LevelNumId s_current_level = LEVEL_ID0;

enum {
  ROTATION_CCW = -1,  // counter-clockwise - UP button - add ANGLE_CHANGE
  ROTATION_NONE = 0,  // none - don't change
  ROTATION_CW = 1     // clockwise - DOWN button - subtract ANGLE_CHANGE
};
static int direction = ROTATION_NONE;

typedef enum {
  RACE_STATUS_NONE,
  RACE_STATUS_COUNTDOWN,
  RACE_STATUS_STARTED,
  RACE_STATUS_PAUSED,
  RACE_STATUS_USER_FINISHED,
  RACE_STATUS_ALL_FINISHED,
} RaceStatus;
static RaceStatus s_race_status = RACE_STATUS_NONE;

PGESprite* bg_level = NULL;

Car* car_user = NULL;
Car* car_opp1 = NULL;
Car* car_opp2 = NULL;
Car* car_opp3 = NULL;

Car* car_create(GPoint start_position, int angle, int resource_white, int resource_black, int resource_color) {
  Car *car_ptr = malloc(sizeof(Car));
  memset(car_ptr, 0x00, sizeof(Car));
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
  // Stop the car movement if current lap > # laps set
  if (car_ptr->lap > s_num_laps) {
    // Update placement if car passes finish line
    if (car_ptr->moving) {
      update_placements(car_ptr);
    }
    car_ptr->moving = false;
  }

  if(car_ptr->moving) {
    GPoint pos = pge_sprite_get_position(car_ptr->sprite_color);

    // Check if car collides with any of the walls - use the center point of each of the four walls
    // of the sprite to check for the direction of the collision
    GRect car_bounds = pge_sprite_get_bounds(car_ptr->sprite_color);
    uint8_t allowable_directions = level_collision_walls(s_current_level, car_bounds);

    // Check if car collides with any other car
    if ((car_ptr != car_user) && (car_user->lap <= s_num_laps)) {
      allowable_directions &= level_collision_cars(car_bounds, pge_sprite_get_bounds(car_user->sprite_color));
    }
    if ((car_ptr != car_opp1) && (car_opp1->lap <= s_num_laps)) {
      allowable_directions &= level_collision_cars(car_bounds, pge_sprite_get_bounds(car_opp1->sprite_color));
    }
    if ((car_ptr != car_opp2) && (car_opp2->lap <= s_num_laps)) {
      allowable_directions &= level_collision_cars(car_bounds, pge_sprite_get_bounds(car_opp2->sprite_color));
    }
    if ((car_ptr != car_opp3) && (car_opp3->lap <= s_num_laps)) {
      allowable_directions &= level_collision_cars(car_bounds, pge_sprite_get_bounds(car_opp3->sprite_color));
    }

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

    if (car_ptr != car_user) {
      update_track_point(car_ptr);
    }
  } else {
    set_placement_position(car_ptr);
  }
}

static void update_car_angle(Car* car_ptr, int change) {
  car_ptr->prev_angle = car_ptr->angle;
  if (change < 0) {
    car_ptr->angle = (car_ptr->angle + change + ANGLE_MASK) % ANGLE_MASK;
  } else {
    car_ptr->angle = (car_ptr->angle + change) % ANGLE_MASK;
  }

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "update_car_angle %d", car_ptr->angle);
  pge_sprite_set_rotation(car_ptr->sprite_color, DEG_TO_TRIG_ANGLE(car_ptr->angle));
}

void update_game_bounds() {
  // Update game frame based on the car position such that the car is centered in the frame
  Layer *game_layer = pge_get_canvas();
  GRect car_bounds = pge_sprite_get_bounds(car_user->sprite_color);
  s_game_bounds = GRect(-(car_bounds.origin.x - (SCREEN_RES_COLS / 2)), -(car_bounds.origin.y - (SCREEN_RES_ROWS / 2)),
                            SCREEN_RES_COLS, SCREEN_RES_ROWS);

  layer_set_bounds(game_layer, s_game_bounds);

  // Update status layer location
  GRect status_rect = GRect(car_bounds.origin.x - (SCREEN_RES_COLS / 2), car_bounds.origin.y + (SCREEN_RES_ROWS / 2) - STATUS_LAYER_HEIGHT,
                            SCREEN_RES_COLS, STATUS_LAYER_HEIGHT);
  layer_set_frame(text_layer_get_layer(s_status_layer), status_rect);
}

#define MIN(a, b) ((a) < (b)) ? (a) : (b)
#define MAX(a, b) ((a) > (b)) ? (a) : (b)

void update_status_layer() {
  if (s_race_status == RACE_STATUS_NONE) {
    snprintf(s_status_text, sizeof(s_status_text), "Press Select");
  } else if (s_race_status == RACE_STATUS_COUNTDOWN) {
    snprintf(s_status_text, sizeof(s_status_text), "Sprint in %d seconds", s_countdown);
  } else if (s_race_status == RACE_STATUS_STARTED) {
    int lap_disp = MIN(s_num_laps, car_user->lap); // Don't go past num laps
    snprintf(s_status_text, sizeof(s_status_text), "Lap: %d of %d", lap_disp, s_num_laps);
  } else if ((s_race_status == RACE_STATUS_USER_FINISHED) || (s_race_status == RACE_STATUS_ALL_FINISHED)) {
    if (car_user->placement == 1) {
      snprintf(s_status_text, sizeof(s_status_text), "1st Place!");
    } else if (car_user->placement == 2) {
      snprintf(s_status_text, sizeof(s_status_text), "2nd Place!");
    } else if (car_user->placement == 3) {
      snprintf(s_status_text, sizeof(s_status_text), "3rd Place!");
    } else if (car_user->placement > 3) {
      snprintf(s_status_text, sizeof(s_status_text), "You Lost :(");
    }
  }
}

static void update_user_car() {
  // Check if crossed checkpoint
  update_checkpoints(car_user);

  // Check if user car crossed finish line and update lap
  update_user_lap(car_user);

  // Update user car angle
  if (pge_get_button_state(BUTTON_ID_UP)) {
    direction = ROTATION_CCW;
    update_car_angle(car_user, direction * ANGLE_CHANGE); 
  } else if (pge_get_button_state(BUTTON_ID_DOWN)) {
    direction = ROTATION_CW;
    update_car_angle(car_user, direction * ANGLE_CHANGE); 
  } else {
    direction = ROTATION_NONE;
  }

  update_car_position(car_user);

  // Update what is visually seen on screen
  update_game_bounds();
}

static void update_opp_cars() {
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "opp: %d %d %d %d", (int)car_opp1->pos_x, (int)car_opp1->pos_y, car_opp1->moving, car_opp1->angle);
  // Update oppenent car angles
  update_car_angle_opp(car_opp1);
  update_car_angle_opp(car_opp2);
  update_car_angle_opp(car_opp3);

  update_car_position(car_opp1);
  update_car_position(car_opp2);
  update_car_position(car_opp3);
}

static void update_countdown(void *context) {
  s_countdown--;
  if (s_countdown > 0) {
    s_countdown_timer = app_timer_register(1000, update_countdown, NULL);
  }
  update_signal(s_countdown);
}

static void logic() {
  if ((s_race_status == RACE_STATUS_COUNTDOWN) && (s_countdown == 3) && (s_countdown_timer == NULL)) {
    // Kick off timer to start race
    update_signal(s_countdown);
    s_countdown_timer = app_timer_register(1000, update_countdown, NULL);
  }
  if ((s_race_status == RACE_STATUS_COUNTDOWN) && (s_countdown == 0)) {
    // Start race
    car_user->moving = true;
    car_opp1->moving = true;
    car_opp2->moving = true;
    car_opp3->moving = true;
    s_race_status = RACE_STATUS_STARTED;
  } else if (s_race_status == RACE_STATUS_STARTED) {
    // Update race status based on whether users and all cars have finished the race
    if ((car_opp1->lap > s_num_laps) && (car_opp2->lap > s_num_laps) &&
        (car_opp3->lap > s_num_laps) && (car_user->lap > s_num_laps)) {
      s_race_status = RACE_STATUS_ALL_FINISHED;
      update_signal(COUNTDOWN_MAX);
    } else if (car_user->lap > s_num_laps) {
      s_race_status = RACE_STATUS_USER_FINISHED;
    }
  } else if (s_race_status == RACE_STATUS_USER_FINISHED) {
    if ((car_opp1->lap > s_num_laps) && (car_opp2->lap > s_num_laps) &&
        (car_opp3->lap > s_num_laps) && (car_user->lap > s_num_laps)) {
      s_race_status = RACE_STATUS_ALL_FINISHED;
      update_signal(COUNTDOWN_MAX);
    }
  }

  if ((s_race_status != RACE_STATUS_COUNTDOWN) && 
      (s_race_status != RACE_STATUS_NONE)) {
    update_opp_cars();
    update_user_car();
  }

  update_status_layer();
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
}

static void click(int button_id, bool long_click) {
  switch(button_id) {
    case BUTTON_ID_UP:
      if (s_race_status == RACE_STATUS_STARTED) {
        direction = ROTATION_CCW;
        update_car_angle(car_user, direction * ANGLE_CHANGE); 
      }
      break;

    case BUTTON_ID_SELECT:
      if (s_race_status == RACE_STATUS_NONE) {
        s_race_status = RACE_STATUS_COUNTDOWN;
      }
#ifdef DEBUG
      car_user->moving = !car_user->moving;
#endif
      break;

    case BUTTON_ID_DOWN:
      if (s_race_status == RACE_STATUS_STARTED) {
        direction = ROTATION_CW;
        update_car_angle(car_user, direction * ANGLE_CHANGE); 
      }
      break;
  }
}

/******************************** Title ***************************************/

void game_deinit() {
  if (!s_game_initialized) {
    return;
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Ending Game...");
  text_layer_destroy(s_game_info_layer);
  text_layer_destroy(s_status_layer);

  level_deinitialize();

  car_destroy(car_user);
  car_user = NULL;
  car_destroy(car_opp1);
  car_opp1 = NULL;
  car_destroy(car_opp2);
  car_opp2 = NULL;
  car_destroy(car_opp3);
  car_opp3 = NULL;

  if (!s_countdown_timer) {
    app_timer_cancel(s_countdown_timer);
    s_countdown_timer = NULL;
  }

  s_race_status = RACE_STATUS_NONE;
  s_game_initialized = false;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Game Ended");

  // Destroy all game resources
  pge_finish();
}

void game_init() {
  game_deinit();
  
  car_speed = CAR_SPEED_DEFAULT;

  s_game_window = pge_begin(GColorBlack, logic, draw, click);
  pge_set_framerate(20);

  Layer *game_layer = pge_get_canvas();

  // Initialize Level
  s_current_level = LEVEL_ID0;
  level_initialize(game_layer, s_current_level);

  s_game_info_layer = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { 144, 20 } });
  text_layer_set_text(s_game_info_layer, s_game_info_buffer);
  text_layer_set_text_alignment(s_game_info_layer, GTextAlignmentCenter);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Creating Cars");

  car_opp1 = car_create((GPoint){.x = 120, .y = 60}, 0,
                        RESOURCE_ID_CAR_RED, RESOURCE_ID_CAR_RED, RESOURCE_ID_CAR_RED);

  car_user = car_create((GPoint){.x = 145, .y = 60}, 0,
                        RESOURCE_ID_CAR_BLUE, RESOURCE_ID_CAR_BLUE, RESOURCE_ID_CAR_BLUE);

  car_opp3 = car_create((GPoint){.x = 120, .y = 82}, 0,
                        RESOURCE_ID_CAR_ORANGE, RESOURCE_ID_CAR_ORANGE, RESOURCE_ID_CAR_ORANGE);

  car_opp2 = car_create((GPoint){.x = 145, .y = 82}, 0,
                        RESOURCE_ID_CAR_PURPLE, RESOURCE_ID_CAR_PURPLE, RESOURCE_ID_CAR_PURPLE);

  update_car_angle(car_user, -90);
  update_car_angle(car_opp1, -90);
  update_car_angle(car_opp2, -90);
  update_car_angle(car_opp3, -90);

  // Update positions of each car
  GPoint pos = pge_sprite_get_position(car_user->sprite_color);
  car_user->pos_x = pos.x;
  car_user->pos_y = pos.y;
  pos = pge_sprite_get_position(car_opp1->sprite_color);
  car_opp1->pos_x = pos.x;
  car_opp1->pos_y = pos.y;
  pos = pge_sprite_get_position(car_opp2->sprite_color);
  car_opp2->pos_x = pos.x;
  car_opp2->pos_y = pos.y;
  pos = pge_sprite_get_position(car_opp3->sprite_color);
  car_opp3->pos_x = pos.x;
  car_opp3->pos_y = pos.y;

  car_user->lap = 1;
  car_opp1->lap = 1;
  car_opp2->lap = 1;
  car_opp3->lap = 1;

  car_user->track_point_offset = 0; // Does not apply anyways
  car_opp1->track_point_offset = -5;
  car_opp2->track_point_offset = 0;
  car_opp3->track_point_offset = 5;

  s_status_layer = text_layer_create(STATUS_LAYER_RECT);
  text_layer_set_background_color(s_status_layer, GColorBlack);
  text_layer_set_text_color(s_status_layer, GColorWhite);
  text_layer_set_text_alignment(s_status_layer, GTextAlignmentCenter);
  text_layer_set_text(s_status_layer, s_status_text);
  update_status_layer();
  layer_add_child(game_layer, text_layer_get_layer(s_status_layer));

  // Updates position of status layer as well
  update_game_bounds();

  s_race_status = RACE_STATUS_NONE;
  s_countdown_timer = NULL;
  s_countdown = 3;

  if (car_speed == 0) {
    car_speed = CAR_SPEED_DEFAULT;
  }

  s_game_initialized = true;

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Game initialized");

}

/******************************** Title Window *****************************************/
extern void level_selector_window_push();

static void title_click(int button_id, bool long_click) {
  switch(button_id) {
    case BUTTON_ID_UP:
      level_selector_window_push();
      break;

    case BUTTON_ID_DOWN:
      break;
  }
}

/******************************** App *****************************************/

static void pebble_sprint_init(void) {
  //pge_title_push("Pebble\nSprint", "SPEED >", "PLAY >", GColorBlack, 0, title_click);
  title_push(0, title_click);
  s_title_pushed = true;
  //pge_set_background(RESOURCE_ID_BG_LEVEL_0);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Main Window Loaded--");
}

void pge_init() {
  //srand(time(NULL));
  car_speed = CAR_SPEED_DEFAULT;

  s_game_initialized = false;
  pebble_sprint_init();
  //pge_splash_show(pebble_sprint_init);
}

void pge_deinit() {
  game_deinit();

  if (s_title_pushed) {
    title_pop();
    s_title_pushed = false;
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Main Window UnLoaded");
}


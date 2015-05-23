#include <pebble.h>

#include <inttypes.h>
#include <math.h>

#include "sprintracer_common.h"
#include "sprintracer_level.h"
#include "sprintracer_title.h"

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

static unsigned int s_num_laps = NUM_LAPS_DEFAULT;

#define STATUS_LAYER_HEIGHT 16
#define STATUS_LAYER_RECT GRect(0, SCREEN_RES_ROWS - STATUS_LAYER_HEIGHT, SCREEN_RES_COLS, STATUS_LAYER_HEIGHT)
static TextLayer *s_status_layer;
static char s_status_text[30];

#define COUNTDOWN_MAX -1
static int s_countdown = 3;
static AppTimer *s_countdown_timer;

static LevelNumId s_current_level = LEVEL0_ID;

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
  RACE_STATUS_FINAL_SCORE,
} RaceStatus;
static RaceStatus s_race_status = RACE_STATUS_NONE;

typedef enum {
  GAME_MODE_NONE,
  GAME_MODE_SINGLE_RACE,
  GAME_MODE_TOURNAMENT,
} GameMode;
static GameMode s_game_mode = GAME_MODE_NONE;

PGESprite* bg_level = NULL;

Car* car_user = NULL;
Car* car_opp1 = NULL;
Car* car_opp2 = NULL;
Car* car_opp3 = NULL;

extern ButtonActionIds *get_button_actions();

static unsigned int s_scores[4]; // 0 = user, 1 = opp1, 2 = opp2, 3 = opp3
void update_tournament_score(Car* car_ptr) {
  // Only update in case of tournament mode
  if (s_game_mode != GAME_MODE_TOURNAMENT) {
    return;
  }

  // Find out which car is being updated
  int index = 0;
  if (car_ptr == car_user) {
    index = 0;
  } else if (car_ptr == car_opp1) {
    index = 1;
  } else if (car_ptr == car_opp2) {
    index = 2;
  } else if (car_ptr == car_opp3) {
    index = 3;
  }

  // Update relevant score for given car based on placement
  if (car_ptr->placement == 1) {
    s_scores[index] += 25;
  } else if (car_ptr->placement == 2) {
    s_scores[index] += 18;
  } else if (car_ptr->placement == 3) {
    s_scores[index] += 15;
  } else if (car_ptr->placement == 4) {
    s_scores[index] += 12;
  }
}

static void update_car_angle(Car* car_ptr, int change) {
  car_ptr->prev_angle = car_ptr->angle;
  if (change < 0) {
    car_ptr->angle = (car_ptr->angle + change + ANGLE_MASK) % ANGLE_MASK;
  } else {
    car_ptr->angle = (car_ptr->angle + change) % ANGLE_MASK;
  }

  pge_sprite_set_rotation(car_ptr->sprite_color, DEG_TO_TRIG_ANGLE(car_ptr->angle));
}

Car* car_create(GPoint start_position, int angle, int resource_white, int resource_black, int resource_color) {
  Car *car_ptr = malloc(sizeof(Car));
  memset(car_ptr, 0x00, sizeof(Car));
  car_ptr->x_change = 0;
  car_ptr->y_change = 0;
  car_ptr->sprite_color = pge_sprite_create(start_position, resource_color);
#ifndef PBL_COLOR
  car_ptr->sprite_white = pge_sprite_create(start_position, resource_white);
  car_ptr->sprite_black = pge_sprite_create(start_position, resource_black);
#endif
  car_ptr->moving = false;
  car_ptr->resource_id = resource_white;
  update_car_angle(car_ptr, angle);
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
    uint8_t allowable_directions;

    if (car_ptr == car_user) {
      allowable_directions = level_collision_walls(car_bounds);
    } else {
      allowable_directions = DIRECTION_ALL;
    }

    // Check if car collides with any other car
    if ((car_ptr != car_user) && (car_user->lap <= s_num_laps)) {
      allowable_directions &= level_collision_cars(car_bounds, car_user);
    }
    if ((car_ptr != car_opp1) && (car_opp1->lap <= s_num_laps)) {
      allowable_directions &= level_collision_cars(car_bounds, car_opp1);
    }
    if ((car_ptr != car_opp2) && (car_opp2->lap <= s_num_laps)) {
      allowable_directions &= level_collision_cars(car_bounds, car_opp2);
    }
    if ((car_ptr != car_opp3) && (car_opp3->lap <= s_num_laps)) {
      allowable_directions &= level_collision_cars(car_bounds, car_opp3);
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

void update_game_bounds(bool force) {
  // Update game frame based on the car position such that the car is centered in the frame
  Layer *game_layer = pge_get_canvas();
  GRect status_rect;

  GRect car_bounds = pge_sprite_get_bounds(car_user->sprite_color);

  if (!force) {
    s_game_bounds = GRect(-(car_bounds.origin.x - (SCREEN_RES_COLS / 2)), -(car_bounds.origin.y - (SCREEN_RES_ROWS / 2)),
                              SCREEN_RES_COLS, SCREEN_RES_ROWS);
    status_rect = GRect(car_bounds.origin.x - (SCREEN_RES_COLS / 2), car_bounds.origin.y + (SCREEN_RES_ROWS / 2) - STATUS_LAYER_HEIGHT,
                              SCREEN_RES_COLS, STATUS_LAYER_HEIGHT);
  } else {
    s_game_bounds = GRect(0, 0,SCREEN_RES_COLS, SCREEN_RES_ROWS);
    status_rect = GRect(0, SCREEN_RES_ROWS - STATUS_LAYER_HEIGHT, SCREEN_RES_COLS, STATUS_LAYER_HEIGHT);
  }

  layer_set_bounds(game_layer, s_game_bounds);

  // Update status layer location
  layer_set_frame(text_layer_get_layer(s_status_layer), status_rect);
}

#define MIN(a, b) ((a) < (b)) ? (a) : (b)
#define MAX(a, b) ((a) > (b)) ? (a) : (b)

void update_status_layer() {
  if ((s_race_status == RACE_STATUS_NONE) || (s_race_status == RACE_STATUS_FINAL_SCORE)) {
    snprintf(s_status_text, sizeof(s_status_text), "Press Select");
  } else if (s_race_status == RACE_STATUS_COUNTDOWN) {
    snprintf(s_status_text, sizeof(s_status_text), "Sprint in %d seconds", s_countdown);
  } else if (s_race_status == RACE_STATUS_STARTED) {
    int lap_disp = MIN(s_num_laps, car_user->lap); // Don't go past num laps
    snprintf(s_status_text, sizeof(s_status_text), "Lap: %d of %d", lap_disp, s_num_laps);
  } else if (((s_race_status == RACE_STATUS_USER_FINISHED) || (s_race_status == RACE_STATUS_ALL_FINISHED)) && (s_game_mode == GAME_MODE_TOURNAMENT)) {
    if (car_user->placement == 1) {
      snprintf(s_status_text, sizeof(s_status_text), "1st Place! Score: %d", s_scores[0]);
    } else if (car_user->placement == 2) {
      snprintf(s_status_text, sizeof(s_status_text), "2nd Place! Score: %d", s_scores[0]);
    } else if (car_user->placement == 3) {
      snprintf(s_status_text, sizeof(s_status_text), "3rd Place! Score: %d", s_scores[0]);
    } else if (car_user->placement > 3) {
      snprintf(s_status_text, sizeof(s_status_text), "You Lost :( Score: %d", s_scores[0]);
    }
  } else if (((s_race_status == RACE_STATUS_USER_FINISHED) || (s_race_status == RACE_STATUS_ALL_FINISHED)) && (s_game_mode == GAME_MODE_SINGLE_RACE)) {
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

  ButtonActionIds *button_actions = get_button_actions();
  // Update user car angle
  if (pge_get_button_state(button_actions->counterclockwise)) {
    direction = ROTATION_CCW;
    update_car_angle(car_user, direction * ANGLE_CHANGE); 
  } else if (pge_get_button_state(button_actions->clockwise)) {
    direction = ROTATION_CW;
    update_car_angle(car_user, direction * ANGLE_CHANGE); 
  } else {
    direction = ROTATION_NONE;
  }

  update_car_position(car_user);

  cannonball_update(car_user, car_opp1, car_opp2, car_opp3);

  // Update what is visually seen on screen
  update_game_bounds(false);
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
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Starting Race in %d seconds", s_countdown);

  update_signal(s_countdown);
  if (s_countdown > 0) {
    s_countdown_timer = app_timer_register(1000, update_countdown, NULL);
  } else {
    s_countdown_timer = NULL;
  }
}

void sort_scores(unsigned int *final_scores) {
  unsigned int lowest, middle1, middle2, highest;
  unsigned int a = s_scores[0];
  unsigned int b  = s_scores[1];
  unsigned int c  = s_scores[2];
  unsigned int d  = s_scores[3];

  unsigned int low1, low2, high1, high2;

  if (a < b) {
    low1 = a;
    high1 = b;
  } else {
    low1 = b;
    high1 = a;
  }

  if (c < d) {
    low2 = c;
    high2 = d;
  } else {
    low2 = d;
    high2 = c;
  }

  if (low1 < low2) {
    lowest = low1;
    middle1 = low2;
  } else {
    lowest = low2;
    middle1 = low1;
  }

  if (high1 > high2) {
    highest = high1;
    middle2 = high2;
  } else {
    highest = high2;
    middle2 = high1;
  }

  if (middle1 < middle2) {
    final_scores[0] = lowest;
    final_scores[1] = middle1;
    final_scores[2] = middle2;
    final_scores[3] = highest;
  } else {
    final_scores[0] = lowest;
    final_scores[1] = middle2;
    final_scores[2] = middle1;
    final_scores[3] = highest;
  }
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

  if (s_race_status == RACE_STATUS_FINAL_SCORE) {
    update_game_bounds(true);
  } else if ((s_race_status != RACE_STATUS_COUNTDOWN) &&
             (s_race_status != RACE_STATUS_NONE)) {
    update_opp_cars();
    update_user_car();
  }

  update_status_layer();
}

void draw(GContext *ctx) {
  if (s_race_status == RACE_STATUS_FINAL_SCORE) {
    // Draw Final Scoreboard
    graphics_context_set_fill_color(ctx, GColorJaegerGreen);
    graphics_fill_rect(ctx, GRect(0, 0, SCREEN_RES_COLS, SCREEN_RES_ROWS), 0, GCornerNone);

    // Draw Cars
    car_user->angle = 0;
    pge_sprite_set_rotation(car_user->sprite_color, DEG_TO_TRIG_ANGLE(0));
    pge_sprite_set_position(car_user->sprite_color, GPoint(32, 54));

    car_opp1->angle = 0;
    pge_sprite_set_rotation(car_opp1->sprite_color, DEG_TO_TRIG_ANGLE(0));
    pge_sprite_set_position(car_opp1->sprite_color, GPoint(32, 78));

    car_opp2->angle = 0;
    pge_sprite_set_rotation(car_opp2->sprite_color, DEG_TO_TRIG_ANGLE(0));
    pge_sprite_set_position(car_opp2->sprite_color, GPoint(32, 102));

    car_opp3->angle = 0;
    pge_sprite_set_rotation(car_opp3->sprite_color, DEG_TO_TRIG_ANGLE(0));
    pge_sprite_set_position(car_opp3->sprite_color, GPoint(32, 126));

    car_draw(car_user, ctx);
    car_draw(car_opp1, ctx);
    car_draw(car_opp2, ctx);
    car_draw(car_opp3, ctx);

    // Draw Scores

    char buff[30];
//    #ifdef DEBUG
//    s_scores[1] = 4;
//    s_scores[2] = 3;
//    s_scores[0] = 2;
//    s_scores[3] = 1;
//    #endif
    unsigned int final_scores[4];
    sort_scores(final_scores);
    GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
    if (s_scores[0] == final_scores[3]) {
      snprintf(buff, sizeof(buff), "You came in\n1st place!");
    } else if (s_scores[0] == final_scores[2]) {
      snprintf(buff, sizeof(buff), "You came in\n2nd place!");
    } else if (s_scores[0] == final_scores[1]) {
      snprintf(buff, sizeof(buff), "You came in\n3rd place!");
    } else if (s_scores[0] == final_scores[0]) {
      snprintf(buff, sizeof(buff), "You came in\nlast place :(");
    }
    graphics_context_set_text_color(ctx, GColorWhite);
    graphics_draw_text(ctx, buff, font, GRect(4, 4, 140, 52), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

    snprintf(buff, sizeof(buff), "Score: %d", s_scores[0]);
    graphics_draw_text(ctx, buff, font, GRect(32, 50, 100, 26), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    snprintf(buff, sizeof(buff), "Score: %d", s_scores[1]);
    graphics_draw_text(ctx, buff, font, GRect(32, 74, 100, 26), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    snprintf(buff, sizeof(buff), "Score: %d", s_scores[2]);
    graphics_draw_text(ctx, buff, font, GRect(32, 98, 100, 26), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    snprintf(buff, sizeof(buff), "Score: %d", s_scores[3]);
    graphics_draw_text(ctx, buff, font, GRect(32, 122, 100, 26), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  } else {
    // Clear board
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_rect(ctx, GRect(0, 0, SCREEN_RES_COLS, SCREEN_RES_ROWS), 0, GCornerNone);

    level_draw(ctx, s_game_bounds);

    car_draw(car_user, ctx);
    car_draw(car_opp1, ctx);
    car_draw(car_opp2, ctx);
    car_draw(car_opp3, ctx);
  }
}

static void click(int button_id, bool long_click) {
  ButtonActionIds *button_actions = get_button_actions();

  if (button_id == button_actions->counterclockwise)
  {
    if (s_race_status == RACE_STATUS_STARTED) {
      direction = ROTATION_CCW;
      update_car_angle(car_user, direction * ANGLE_CHANGE); 
    }
  } else if (button_id == button_actions->select) {
    if (s_race_status == RACE_STATUS_NONE) {
      s_race_status = RACE_STATUS_COUNTDOWN;
    } else if ((s_race_status == RACE_STATUS_ALL_FINISHED) && (s_game_mode == GAME_MODE_TOURNAMENT)) {
      LevelNumId current_level = level_get_current();
      current_level = (current_level + 1) % LEVEL_COUNT;
      if (current_level == LEVEL0_ID) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Printing Final Scores");
        s_race_status = RACE_STATUS_FINAL_SCORE;
      } else {
        level_set_current(current_level);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Starting next level %d", current_level);
        game_init();
      }
    } else if ((s_race_status == RACE_STATUS_FINAL_SCORE) && (s_game_mode == GAME_MODE_TOURNAMENT)) {
      if ((int)s_scores[0] > pge_title_get_highscore()) {
        pge_title_set_highscore(s_scores[0]);
      }
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Ending Tournament");
      game_deinit();
    } else if ((s_race_status == RACE_STATUS_ALL_FINISHED) && (s_game_mode == GAME_MODE_SINGLE_RACE)) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Ending Single Race");
      game_deinit();
    } else {
      cannonball_fire(car_user);
    }
#ifdef DEBUG
    car_user->moving = !car_user->moving;
    car_opp1->moving = !car_opp1->moving;
    car_opp2->moving = !car_opp2->moving;
    car_opp3->moving = !car_opp3->moving;
#endif
  }
//#ifdef DEBUG
//  else if (button_id == button_actions->clockwise) {
//    s_race_status = RACE_STATUS_FINAL_SCORE;
//  } 
//#else
  else if (button_id == button_actions->clockwise) {
    if (s_race_status == RACE_STATUS_STARTED) {
      direction = ROTATION_CW;
      update_car_angle(car_user, direction * ANGLE_CHANGE); 
    }
  }
//#endif
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

  if (s_countdown_timer) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Cancelling s_countdown_timer = %p", (void*)s_countdown_timer);
    app_timer_cancel(s_countdown_timer);
    s_countdown_timer = NULL;
  }

  s_race_status = RACE_STATUS_NONE;
  s_game_initialized = false;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Game Ended");

  // Destroy all game resources
  pge_finish();

  if (s_game_window && window_stack_contains_window(s_game_window)) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Explicitly removing game window from stack");
    window_stack_remove(s_game_window, false);
    s_game_window = NULL;
  }
}

extern int get_car_selection(int index);

void game_init() {
  game_deinit();
  
  car_speed = CAR_SPEED_DEFAULT;

  s_game_window = pge_begin(GColorBlack, logic, draw, click);
  pge_set_framerate(20);

  Layer *game_layer = pge_get_canvas();

  // Initialize Level
  s_current_level = level_get_current();
  level_initialize(game_layer, s_current_level);

  s_game_info_layer = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { 144, 20 } });
  text_layer_set_text(s_game_info_layer, s_game_info_buffer);
  text_layer_set_text_alignment(s_game_info_layer, GTextAlignmentCenter);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Creating Cars");

  int starting_angle = get_starting_angle();
  int car_selection = get_car_selection(0);
  car_user = car_create(get_starting_location(0), starting_angle,
                        car_selection, car_selection, car_selection);

  car_selection = get_car_selection(1);
  car_opp1 = car_create(get_starting_location(1), starting_angle,
                        car_selection, car_selection, car_selection);

  car_selection = get_car_selection(2);
  car_opp2 = car_create(get_starting_location(2), starting_angle,
                        car_selection, car_selection, car_selection);

  car_selection = get_car_selection(3);
  car_opp3 = car_create(get_starting_location(3), starting_angle,
                        car_selection, car_selection, car_selection);

  car_user->track_point_offset = 0; // Does not apply anyways
  car_opp1->track_point_offset = 0;
  car_opp2->track_point_offset = -5;
  car_opp3->track_point_offset = 5;

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

  s_status_layer = text_layer_create(STATUS_LAYER_RECT);
  text_layer_set_background_color(s_status_layer, GColorBlack);
  text_layer_set_text_color(s_status_layer, GColorWhite);
  text_layer_set_text_alignment(s_status_layer, GTextAlignmentCenter);
  text_layer_set_text(s_status_layer, s_status_text);
  update_status_layer();
  layer_add_child(game_layer, text_layer_get_layer(s_status_layer));

  // Updates position of status layer as well
  update_game_bounds(false);

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
extern void settings_window_push();
extern void car_selector_window_push();

void title_click(int button_id, bool long_click) {
  switch(button_id) {
    case BUTTON_ID_UP:
      s_game_mode = GAME_MODE_SINGLE_RACE;
      level_selector_window_push();
      break;

    case BUTTON_ID_SELECT:
      s_game_mode = GAME_MODE_TOURNAMENT;
      s_scores[0] = 0;
      s_scores[1] = 0;
      s_scores[2] = 0;
      s_scores[3] = 0;
      level_set_current(LEVEL0_ID);
      car_selector_window_push();
      break;

    case BUTTON_ID_DOWN:
      s_game_mode = GAME_MODE_NONE;
      settings_window_push();
      break;
  }
}

/******************************** App *****************************************/
void pge_init() {
  //srand(time(NULL));
  car_speed = CAR_SPEED_DEFAULT;

  s_game_initialized = false;
  title_push(title_click);
  s_title_pushed = true;

  set_button_actions(BUTTON_ID_UP, BUTTON_ID_SELECT, BUTTON_ID_DOWN);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Main Window Loaded--");
}

void pge_deinit() {
  game_deinit();

  if (s_title_pushed) {
    title_pop();
    s_title_pushed = false;
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Main Window UnLoaded");
}


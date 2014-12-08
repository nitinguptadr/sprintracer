#include <pebble.h>

#include "pge/pge.h"
#include "pge/additional/pge_sprite.h"
#include "pge/additional/pge_title.h"
#include "pge/additional/pge_grid.h"
#include "pge/additional/pge_collision.h"

#define DIRECTION_0      0
#define DIRECTION_45    45
#define DIRECTION_90    90
#define DIRECTION_135  135
#define DIRECTION_180  180
#define DIRECTION_225  225
#define DIRECTION_270  270
#define DIRECTION_315  315

#define NUM_DIRECTIONS 8

// Car dimensions
// 12px by 18 px - 0 degrees
// 18px by 18 px - 45 degrees

#define CAR_SIZE 18
#define CAR_SPEED_DEFAULT 2
#define CAR_SPEED_MAX 3
#define ANGLE_CHANGE 4
#define ANGLE_CHANGE_RESOURCE 45
#define ANGLE_MASK 360

#define SCREEN_RES_ROWS 168
#define SCREEN_RES_COLS 144


static Window *s_main_window;
static TextLayer *text_layer;
#define TEXT_BUFFER_SIZE 20
char textbuffer[TEXT_BUFFER_SIZE];
static uint8_t count_down = 3;
static uint8_t car_speed = CAR_SPEED_DEFAULT;

enum {
  DIRECTION_CCW = -1,  // counter-clockwise - UP button - subtract ANGLE_CHANGE
  DIRECTION_NONE = 0,  // none - don't change
  DIRECTION_CW = 1     // clockwise - DOWN button - add ANGLE_CHANGE
};
static int direction = DIRECTION_NONE;

typedef struct {
  PGESprite *sprite_white;
  PGESprite *sprite_black;
  int resource_id;
  bool moving;
  int prev_angle;
  int angle;
  int x_change;
  int y_change;
} Car;

//static int s_direction, s_score;
//static bool s_moving = false;
PGESprite* finish_line = NULL;

Car* carblack = NULL;
Car* carwhite = NULL;
Car* cargray = NULL;
Car* carbox = NULL;
Car* carstripe = NULL;

Car* car_create(GPoint start_position, int angle, int resource_white, int resource_black) {
  Car *car_ptr = malloc(sizeof(Car));
  car_ptr->angle = angle;
  car_ptr->x_change = 0;
  car_ptr->y_change = 0;
  car_ptr->sprite_white = pge_sprite_create(start_position, resource_white);
  car_ptr->sprite_black = pge_sprite_create(start_position, resource_black);
  car_ptr->moving = false;
  car_ptr->resource_id = resource_white;
  return car_ptr;
}

void car_destroy(Car* car_ptr) {
  if (car_ptr) {
    pge_sprite_destroy(car_ptr->sprite_white);
    pge_sprite_destroy(car_ptr->sprite_black); 
    free(car_ptr);
    car_ptr = NULL;
  }
}

void car_draw(Car* car_ptr, GContext *ctx) {
  graphics_context_set_compositing_mode(ctx, GCompOpClear);
  pge_sprite_draw(car_ptr->sprite_black, ctx);

  graphics_context_set_compositing_mode(ctx, GCompOpOr);
  pge_sprite_draw(car_ptr->sprite_white, ctx);
}

static void update_car_position(Car* car_ptr) {
  GPoint pos = pge_sprite_get_position(car_ptr->sprite_white);

  // Move square and wrap to screen
  if(car_ptr->moving) {
    car_ptr->x_change += sin_lookup(car_ptr->angle * TRIG_MAX_ANGLE / ANGLE_MASK);
    if (car_ptr->x_change > TRIG_MAX_ANGLE) {
      pos.x += car_speed * (car_ptr->x_change / TRIG_MAX_ANGLE);
      car_ptr->x_change -= TRIG_MAX_ANGLE;
    } else if (car_ptr->x_change < -TRIG_MAX_ANGLE) {
      pos.x += car_speed * (car_ptr->x_change / TRIG_MAX_ANGLE);
      car_ptr->x_change += TRIG_MAX_ANGLE;
    }

    car_ptr->y_change += -cos_lookup(car_ptr->angle * TRIG_MAX_ANGLE / ANGLE_MASK);
    if (car_ptr->y_change > TRIG_MAX_ANGLE) {
      pos.y += car_speed * (car_ptr->y_change / TRIG_MAX_ANGLE);
      car_ptr->y_change -= TRIG_MAX_ANGLE;
    } else if (car_ptr->y_change < -TRIG_MAX_ANGLE) {
      pos.y += car_speed * (car_ptr->y_change / TRIG_MAX_ANGLE);
      car_ptr->y_change += TRIG_MAX_ANGLE;
    }

    if(pos.x < 0) {
      pos.x = 0;
    } else if (pos.x > SCREEN_RES_COLS - CAR_SIZE) {
      pos.x = SCREEN_RES_COLS - CAR_SIZE;
    }

    if(pos.y < 0) {
      pos.y = 0;
    } else if (pos.y > SCREEN_RES_ROWS - CAR_SIZE) {
      pos.y = SCREEN_RES_ROWS - CAR_SIZE;
    }

    // Update sprite
    pge_sprite_set_position(car_ptr->sprite_black, pos);
    pge_sprite_set_position(car_ptr->sprite_white, pos);
  }
}

static void update_car_angle(Car* car_ptr, int change) {
  int resource_id = 0; // take advantage of the known relative resource id positions in your json file
  car_ptr->prev_angle = car_ptr->angle;
  if (change < 0) {
    car_ptr->angle = (carblack->angle + change + ANGLE_MASK) % ANGLE_MASK;
  } else {
    car_ptr->angle = (car_ptr->angle + change) % ANGLE_MASK;
  }

  //car_ptr->x_change = 0;
  //car_ptr->y_change = 0;
  resource_id = car_ptr->resource_id + (((car_ptr->angle / ANGLE_CHANGE_RESOURCE) * 2) % (2*NUM_DIRECTIONS));
  APP_LOG(APP_LOG_LEVEL_DEBUG, "angle: %d, resource ID: %d", car_ptr->angle, resource_id);
  pge_sprite_set_frame(car_ptr->sprite_white, resource_id);  
  pge_sprite_set_frame(car_ptr->sprite_black, resource_id + 1);  
}

static void loop() {
  update_car_position(carblack);
  update_car_position(carwhite);
  update_car_position(cargray);
  update_car_position(carbox);

  if (pge_get_button_state(BUTTON_ID_UP)) {
    direction = DIRECTION_CCW;
  } else if (pge_get_button_state(BUTTON_ID_DOWN)) {
    direction = DIRECTION_CW;
  } else {
    direction = DIRECTION_NONE;
  }

  if (direction != DIRECTION_NONE) {
    update_car_angle(carblack, direction * ANGLE_CHANGE); 
  }
}

void draw(GContext *ctx) {
  //snprintf(textbuffer, sizeof(textbuffer), "S:%d,A:%d,X:%d,Y%d", car_speed, carblack->angle, carblack->x_change, carblack->y_change);
  snprintf(textbuffer, sizeof(textbuffer), "X:%d,Y%d", carblack->x_change, carblack->y_change);
  text_layer_set_text(text_layer, textbuffer);

  pge_sprite_draw(finish_line, ctx);

  car_draw(carblack, ctx);
  car_draw(carwhite, ctx);
  car_draw(cargray, ctx);
  car_draw(carbox, ctx);
}

// Optional, can be NULL if only using pge_get_button_state()
static void click(int button_id) {
  // Change direction - clockwise 0 - 3
  switch(button_id) {
  case BUTTON_ID_UP:
      direction = DIRECTION_CCW;
      break;

    case BUTTON_ID_SELECT:
      carblack->moving = !carblack->moving;
      //carwhite->moving = !carwhite->moving;
      //cargray->moving = !cargray->moving;
      //carbox->moving = !carbox->moving;
      break;

    case BUTTON_ID_DOWN:
      direction = DIRECTION_CW;
      break;
  }
}

/******************************* Main Window **********************************/

static void main_window_load(Window *window) {
   // Create game canvas and begin render loop
  pge_begin(window, loop, draw, click);

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 22, 72 }, .size = { 120, 20 } });
  layer_add_child(window_layer, text_layer_get_layer(text_layer));

  pge_set_background(RESOURCE_ID_TRACK_CIRCLE);

  finish_line = pge_sprite_create((GPoint){.x = 102, .y = 78}, RESOURCE_ID_FINISH_LINE);
  carblack = car_create((GPoint){.x = 104, .y = 88}, 0, RESOURCE_ID_CAR_BLACK_0_WHITE, RESOURCE_ID_CAR_BLACK_0_BLACK);

  carwhite = car_create((GPoint){.x = 124, .y = 88}, 0, RESOURCE_ID_CAR_WHITE_0_WHITE, RESOURCE_ID_CAR_WHITE_0_BLACK);

  cargray = car_create((GPoint){.x = 86, .y = 108}, 0, RESOURCE_ID_CAR_GRAY_45_WHITE, RESOURCE_ID_CAR_GRAY_45_BLACK);

  carbox = car_create((GPoint){.x = 112, .y = 108}, 0, RESOURCE_ID_CAR_BOX_45_WHITE, RESOURCE_ID_CAR_BOX_45_BLACK);

  carstripe = car_create((GPoint){.x = 112, .y = 60}, 0, RESOURCE_ID_CAR_STRIPE_0_WHITE, RESOURCE_ID_CAR_STRIPE_0_BLACK);

  text_layer_set_text(text_layer, textbuffer);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  //snprintf(textbuffer, sizeof(textbuffer), "S:%d,A:%d,X:%d,Y%d", car_speed, carblack->angle, carblack->x_change, carblack->y_change);
  snprintf(textbuffer, sizeof(textbuffer), "X:%d,Y%d", carblack->x_change, carblack->y_change);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Main Window Loaded");
}

static void main_window_unload(Window *window) {
  text_layer_destroy(text_layer);

  pge_sprite_destroy(finish_line);
  finish_line = NULL;

  car_destroy(carblack);
  carblack = NULL;
  car_destroy(carwhite);
  carwhite = NULL;
  car_destroy(cargray);
  cargray = NULL;
  car_destroy(carbox);
  carbox = NULL;
  car_destroy(carstripe);
  carstripe = NULL;

  // Destroy all game resources
  pge_finish();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Main Window UnLoaded");

}

/******************************** Title ***************************************/

static void title_click(int button_id) {
  switch(button_id) {
    // Some other action
    case BUTTON_ID_SELECT:
      car_speed = (car_speed + 1) % (CAR_SPEED_MAX + 1);
      if (car_speed == 0) {
        car_speed = 1;
      }
      break; 

    case BUTTON_ID_DOWN:
      // Go to game!
      window_stack_push(s_main_window, true);
      break;
  }
}

/******************************** App *****************************************/

static void init(void) {
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  window_set_fullscreen(s_main_window, true);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  pge_title_push("Pebble      Sprint", "SPEED >   ", "PLAY >      ", GColorBlack, RESOURCE_ID_MAIN_MENU_BG, title_click);
}

static void deinit(void) {
  pge_title_pop();

  window_destroy(s_main_window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_main_window);

  app_event_loop();
  deinit();
}

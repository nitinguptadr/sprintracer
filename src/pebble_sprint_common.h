#pragma once

#include "pge/additional/pge_sprite.h"

#define SCREEN_RES_ROWS 168
#define SCREEN_RES_COLS 144

// Mask to determine which direction the car can travel
#define DIRECTION_NONE  0
#define DIRECTION_UP    1
#define DIRECTION_LEFT  2
#define DIRECTION_RIGHT 4
#define DIRECTION_DOWN  8
#define DIRECTION_ALL   (DIRECTION_UP | DIRECTION_LEFT | DIRECTION_RIGHT | DIRECTION_DOWN)

#define DEG_TO_TRIG_ANGLE(angle) (((angle % 360) * TRIG_MAX_ANGLE) / 360)

#define ANGLE_CHANGE 9
#define TRACKPOINT_RADIUS 5

#define NUM_CARS_TOTAL 4

#define CAR_PLACEMENT_UNSET 0

typedef enum {
  LEVEL_ID0,

  // Add new levels above this
  LEVEL_COUNT,
  LEVEL_INVALID
} LevelNumId;

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
  int track_point_index; // Current index on track that car is gravitating towards
  int track_point_offset;  // Number of pixels to offset from each track point
  int lap;               // Current lap number of car
  bool crossing_finish;  // Stores whether crossing finish line or not
  int placement;
} Car;


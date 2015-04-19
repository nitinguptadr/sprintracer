#pragma once

#include <pebble.h>

#include "pebble_sprint_common.h"
#include "pge/additional/pge_sprite.h"

typedef struct {
  int resource_id;
  GPoint offset;
  PGESprite *sprite;
} LevelSpriteLocation;

typedef struct {
  LevelSpriteLocation finish_line;
  LevelSpriteLocation finish_box_1;
  LevelSpriteLocation finish_box_2;
  LevelSpriteLocation finish_box_3;
  LevelSpriteLocation finish_box_4;
  LevelSpriteLocation light_signal;
} LevelFinishLine;


typedef struct {
  LevelNumId level;
  LevelSpriteLocation *sprites;
  LevelSpriteLocation *scenery;
  LevelSpriteLocation *walls;
  LevelSpriteLocation *tracks;
  LevelFinishLine *finish_group;
  GPoint *track_points0;
  Car *finish_order[NUM_CARS_TOTAL];
} LevelSpriteDetails;

// Intializes current level
void level_initialize(Layer *game_layer, uint8_t level);

// Deintializes current level
void level_deinitialize();

// Draw the current level
void level_draw(GContext *ctx, GRect game_bounds);

uint8_t level_collision_walls(LevelNumId level, GRect car_bounds);

uint8_t level_collision_cars(GRect car_bounds, GRect car_bounds_opponent);

void set_placement_position(Car *car_ptr);

void update_placements(Car *car_ptr);

void update_car_angle_opp(Car* car_ptr);

void update_track_point(Car *car_ptr);

void update_car_lap(Car *car_ptr);

void update_signal(int countdown);


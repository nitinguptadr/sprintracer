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
  LevelNumId level;
  LevelSpriteLocation *sprites;
  LevelSpriteLocation *scenery;
  LevelSpriteLocation *walls;
  LevelSpriteLocation *tracks;
  LevelSpriteLocation *finish_line;
  GPoint *track_points0;
} LevelSpriteDetails;

// Intializes current level
void level_initialize(Layer *game_layer, uint8_t level);

// Deintializes current level
void level_deinitialize();

// Draw the current level
void level_draw(GContext *ctx, GRect game_bounds);

uint8_t level_collision_walls(LevelNumId level, GRect car_bounds);

uint8_t level_collision_cars(GRect car_bounds, GRect car_bounds_opponent);

void update_car_angle_opp(Car* car_ptr);

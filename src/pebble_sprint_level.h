#pragma once

#include <pebble.h>

#include "pge/additional/pge_sprite.h"

typedef struct {
  int resource_id;
  GPoint offset;
  PGESprite *sprite;
} LevelSpriteLocation;

void level_initialize(Layer *game_layer, uint8_t level);

void level_deinitialize(uint8_t level);

// Draw level based on the position of the car
void level_draw(GContext *ctx, uint8_t level);

uint8_t level_collision_walls(LevelNumId level, GRect car_bounds);

uint8_t level_collision_cars(GRect car_bounds, GRect car_bounds_opponent);

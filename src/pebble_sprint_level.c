#include <pebble.h>

#include "pebble_sprint_common.h"
#include "pge/additional/pge_sprite.h"
#include "pge/additional/pge_collision.h"
#include "resources/images/level0_sprites.h"

void level_initialize(Layer *game_layer, LevelNumId level) {
  level_grass.sprite = pge_sprite_create(level_grass.offset, level_grass.resource_id);
  switch(level) {
    case LEVEL_ID0:
      layer_set_frame(game_layer, LEVEL0_BOUNDS);
      for (uint32_t index = 0; index < (sizeof(level0_walls) / sizeof(LevelSpriteLocation)); index++) {
        level0_walls[index].sprite = pge_sprite_create(level0_walls[index].offset, level0_walls[index].resource_id);
      }
      for (uint32_t index = 0; index < (sizeof(level0_scenery) / sizeof(LevelSpriteLocation)); index++) {
        level0_scenery[index].sprite = pge_sprite_create(level0_scenery[index].offset, level0_scenery[index].resource_id);
      }
      for (uint32_t index = 0; index < (sizeof(level0_sprites) / sizeof(LevelSpriteLocation)); index++) {
        level0_sprites[index].sprite = pge_sprite_create(level0_sprites[index].offset, level0_sprites[index].resource_id);
      }
      for (uint32_t index = 0; index < (sizeof(level0_track) / sizeof(LevelSpriteLocation)); index++) {
        level0_track[index].sprite = pge_sprite_create(level0_track[index].offset, level0_track[index].resource_id);
      }
      break;
  }
}

void level_deinitialize(LevelNumId level) {
  pge_sprite_destroy(level_grass.sprite);
  switch(level) {
    case LEVEL_ID0:
      for (uint32_t index = 0; index < (sizeof(level0_walls) / sizeof(LevelSpriteLocation)); index++) {
        pge_sprite_destroy(level0_walls[index].sprite);
      }
      for (uint32_t index = 0; index < (sizeof(level0_scenery) / sizeof(LevelSpriteLocation)); index++) {
        pge_sprite_destroy(level0_scenery[index].sprite);
      }
      for (uint32_t index = 0; index < (sizeof(level0_track) / sizeof(LevelSpriteLocation)); index++) {
        pge_sprite_destroy(level0_track[index].sprite);
      }
      for (uint32_t index = 0; index < (sizeof(level0_sprites) / sizeof(LevelSpriteLocation)); index++) {
        pge_sprite_destroy(level0_sprites[index].sprite);
      }
      break;
  }
}

// Draw level based on the position of the car
void level_draw(GContext *ctx, LevelNumId level) {
  switch(level) {
    case LEVEL_ID0:
      //graphics_draw_bitmap_in_rect(ctx, level_grass.sprite->bitmap, LEVEL0_BOUNDS);
#ifdef PBL_COLOR
      graphics_context_set_fill_color(ctx, GColorJaegerGreen);
      graphics_fill_rect(ctx, GRect(-SCREEN_RES_COLS, -SCREEN_RES_ROWS, 4*SCREEN_RES_COLS, 4*SCREEN_RES_ROWS), 0, GCornerNone);
#endif
      graphics_context_set_compositing_mode(ctx, GCompOpAssign);
      for (uint32_t index = 0; index < (sizeof(level0_scenery) / sizeof(LevelSpriteLocation)); index++) {
        pge_sprite_draw(level0_scenery[index].sprite, ctx);
      }
      graphics_context_set_compositing_mode(ctx, GCompOpSet);
      for (uint32_t index = 0; index < (sizeof(level0_sprites) / sizeof(LevelSpriteLocation)); index++) {
        pge_sprite_draw(level0_sprites[index].sprite, ctx);
      }
      graphics_context_set_compositing_mode(ctx, GCompOpSet);
      for (uint32_t index = 0; index < (sizeof(level0_walls) / sizeof(LevelSpriteLocation)); index++) {
        pge_sprite_draw(level0_walls[index].sprite, ctx);
      }
      graphics_context_set_compositing_mode(ctx, GCompOpSet);
      for (uint32_t index = 0; index < (sizeof(level0_track) / sizeof(LevelSpriteLocation)); index++) {
        pge_sprite_draw(level0_track[index].sprite, ctx);
      }
      break;
  }
}

#define CAR_BOUNDARY_OFFSET 3
uint8_t level_collision_walls(LevelNumId level, GRect car_bounds) {
  GLine top =    { GPoint(car_bounds.origin.x + CAR_BOUNDARY_OFFSET, car_bounds.origin.y),
                   GPoint(car_bounds.origin.x + car_bounds.size.w - 2*CAR_BOUNDARY_OFFSET, car_bounds.origin.y) };
  GLine left =   { GPoint(car_bounds.origin.x, car_bounds.origin.y + CAR_BOUNDARY_OFFSET),
                   GPoint(car_bounds.origin.x, car_bounds.origin.y + car_bounds.size.h - 2*CAR_BOUNDARY_OFFSET) };
  GLine right =  { GPoint(car_bounds.origin.x + car_bounds.size.w, car_bounds.origin.y + CAR_BOUNDARY_OFFSET),
                   GPoint(car_bounds.origin.x + car_bounds.size.w, car_bounds.origin.y + car_bounds.size.h - 2*CAR_BOUNDARY_OFFSET) };
  GLine bottom = { GPoint(car_bounds.origin.x + CAR_BOUNDARY_OFFSET, car_bounds.origin.y + car_bounds.size.h),
                   GPoint(car_bounds.origin.x + car_bounds.size.w - 2*CAR_BOUNDARY_OFFSET, car_bounds.origin.y + car_bounds.size.h) };

  GPoint top_center = GPoint((top.p1.x + top.p2.x) / 2, (top.p1.y + top.p2.y) / 2);
  GPoint left_center = GPoint((left.p1.x + left.p2.x) / 2, (left.p1.y + left.p2.y) / 2);
  GPoint right_center = GPoint((right.p1.x + right.p2.x) / 2, (right.p1.y + right.p2.y) / 2);
  GPoint bottom_center = GPoint((bottom.p1.x + bottom.p2.x) / 2, (bottom.p1.y + bottom.p2.y) / 2);

  uint8_t allowable_directions = DIRECTION_ALL;

  // Check each wall for the given level against each border of the car - if any collide, mask out
  // the appropriate direction
  switch(level) {
    case LEVEL_ID0:
      for (uint32_t index = 0; index < (sizeof(level0_walls) / sizeof(LevelSpriteLocation)); index++) {
        GRect wall_bounds = pge_sprite_get_bounds(level0_walls[index].sprite);
        if (pge_collision_line_rectangle(&top, &wall_bounds) || pge_collision_point_rectangle(&top_center, &wall_bounds)) {
          allowable_directions &= ~((uint8_t)DIRECTION_UP);
        }
        if (pge_collision_line_rectangle(&left, &wall_bounds) || pge_collision_point_rectangle(&left_center, &wall_bounds)) {
          allowable_directions &= ~((uint8_t)DIRECTION_LEFT);
        }
        if (pge_collision_line_rectangle(&right, &wall_bounds) || pge_collision_point_rectangle(&right_center, &wall_bounds)) {
          allowable_directions &= ~((uint8_t)DIRECTION_RIGHT);
        }
        if (pge_collision_line_rectangle(&bottom, &wall_bounds) || pge_collision_point_rectangle(&bottom_center, &wall_bounds)) {
          allowable_directions &= ~((uint8_t)DIRECTION_DOWN);
        }
      }
      for (uint32_t index = 0; index < (sizeof(level0_sprites) / sizeof(LevelSpriteLocation)); index++) {
        GRect sprite_bounds = pge_sprite_get_bounds(level0_sprites[index].sprite);
        if (pge_collision_line_rectangle(&top, &sprite_bounds) || pge_collision_point_rectangle(&top_center, &sprite_bounds)) {
          allowable_directions &= ~((uint8_t)DIRECTION_UP);
        }
        if (pge_collision_line_rectangle(&left, &sprite_bounds) || pge_collision_point_rectangle(&left_center, &sprite_bounds)) {
          allowable_directions &= ~((uint8_t)DIRECTION_LEFT);
        }
        if (pge_collision_line_rectangle(&right, &sprite_bounds) || pge_collision_point_rectangle(&right_center, &sprite_bounds)) {
          allowable_directions &= ~((uint8_t)DIRECTION_RIGHT);
        }
        if (pge_collision_line_rectangle(&bottom, &sprite_bounds) || pge_collision_point_rectangle(&bottom_center, &sprite_bounds)) {
          allowable_directions &= ~((uint8_t)DIRECTION_DOWN);
        }
      }
      break;
  }

  return allowable_directions;
}

uint8_t level_collision_cars(GRect car_bounds, GRect car_bounds_opponent) {
  GLine top =    { GPoint(car_bounds.origin.x + CAR_BOUNDARY_OFFSET, car_bounds.origin.y),
                   GPoint(car_bounds.origin.x + car_bounds.size.w - 2*CAR_BOUNDARY_OFFSET, car_bounds.origin.y) };
  GLine left =   { GPoint(car_bounds.origin.x, car_bounds.origin.y + CAR_BOUNDARY_OFFSET),
                   GPoint(car_bounds.origin.x, car_bounds.origin.y + car_bounds.size.h - 2*CAR_BOUNDARY_OFFSET) };
  GLine right =  { GPoint(car_bounds.origin.x + car_bounds.size.w, car_bounds.origin.y + CAR_BOUNDARY_OFFSET),
                   GPoint(car_bounds.origin.x + car_bounds.size.w, car_bounds.origin.y + car_bounds.size.h - 2*CAR_BOUNDARY_OFFSET) };
  GLine bottom = { GPoint(car_bounds.origin.x + CAR_BOUNDARY_OFFSET, car_bounds.origin.y + car_bounds.size.h),
                   GPoint(car_bounds.origin.x + car_bounds.size.w - 2*CAR_BOUNDARY_OFFSET, car_bounds.origin.y + car_bounds.size.h) };

  GPoint top_center = GPoint((top.p1.x + top.p2.x) / 2, (top.p1.y + top.p2.y) / 2);
  GPoint left_center = GPoint((left.p1.x + left.p2.x) / 2, (left.p1.y + left.p2.y) / 2);
  GPoint right_center = GPoint((right.p1.x + right.p2.x) / 2, (right.p1.y + right.p2.y) / 2);
  GPoint bottom_center = GPoint((bottom.p1.x + bottom.p2.x) / 2, (bottom.p1.y + bottom.p2.y) / 2);

  uint8_t allowable_directions = DIRECTION_ALL;

  if (pge_collision_line_rectangle(&top, &car_bounds_opponent) || pge_collision_point_rectangle(&top_center, &car_bounds_opponent)) {
    allowable_directions &= ~((uint8_t)DIRECTION_UP);
  }
  if (pge_collision_line_rectangle(&left, &car_bounds_opponent) || pge_collision_point_rectangle(&left_center, &car_bounds_opponent)) {
    allowable_directions &= ~((uint8_t)DIRECTION_LEFT);
  }
  if (pge_collision_line_rectangle(&right, &car_bounds_opponent) || pge_collision_point_rectangle(&right_center, &car_bounds_opponent)) {
    allowable_directions &= ~((uint8_t)DIRECTION_RIGHT);
  }
  if (pge_collision_line_rectangle(&bottom, &car_bounds_opponent) || pge_collision_point_rectangle(&bottom_center, &car_bounds_opponent)) {
    allowable_directions &= ~((uint8_t)DIRECTION_DOWN);
  }

  return allowable_directions;
}



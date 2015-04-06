#include <pebble.h>

#include "pebble_sprint_common.h"
#include "pge/additional/pge_sprite.h"
#include "pge/additional/pge_collision.h"
#include "resources/images/level0_sprites.h"

static LevelSpriteDetails *current_level = NULL;
static uint32_t current_num_scenery = 0;
static uint32_t current_num_sprites = 0;
static uint32_t current_num_walls = 0;
static uint32_t current_num_tracks = 0;
static uint32_t current_num_track_points0 = 0;

void level_initialize(Layer *game_layer, LevelNumId level) {
  switch(level) {
    case LEVEL_ID0:
      layer_set_frame(game_layer, LEVEL0_BOUNDS);
      current_level = &level0_details;
      current_num_scenery = sizeof(level0_scenery) / sizeof(LevelSpriteLocation);
      current_num_sprites = sizeof(level0_sprites) / sizeof(LevelSpriteLocation);
      current_num_walls = sizeof(level0_walls) / sizeof(LevelSpriteLocation);
      current_num_tracks = sizeof(level0_tracks) / sizeof(LevelSpriteLocation);
      current_num_track_points0 = sizeof(level0_track_points0) / sizeof(GPoint);
      break;
    default:
      break;
  }

  if (current_level) {
    for (uint32_t index = 0; index < current_num_scenery; index++) {
      current_level->scenery[index].sprite = pge_sprite_create(current_level->scenery[index].offset, current_level->scenery[index].resource_id);
    }
    for (uint32_t index = 0; index < current_num_sprites; index++) {
      current_level->sprites[index].sprite = pge_sprite_create(current_level->sprites[index].offset, current_level->sprites[index].resource_id);
    }
    for (uint32_t index = 0; index < current_num_walls; index++) {
      current_level->walls[index].sprite = pge_sprite_create(current_level->walls[index].offset, current_level->walls[index].resource_id);
    }
    for (uint32_t index = 0; index < current_num_tracks; index++) {
      current_level->tracks[index].sprite = pge_sprite_create(current_level->tracks[index].offset, current_level->tracks[index].resource_id);
    }

    current_level->finish_line[0].sprite = pge_sprite_create(current_level->finish_line[0].offset, current_level->finish_line[0].resource_id);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Level %d initialized", level);
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Invalid level %d. Could not initialize", level);
  }
}

void level_deinitialize() {
  if (current_level) {
    for (uint32_t index = 0; index < current_num_scenery; index++) {
      pge_sprite_destroy(current_level->scenery[index].sprite);
    }
    for (uint32_t index = 0; index < current_num_sprites; index++) {
      pge_sprite_destroy(current_level->sprites[index].sprite);
    }
    for (uint32_t index = 0; index < current_num_walls; index++) {
      pge_sprite_destroy(current_level->walls[index].sprite);
    }
    for (uint32_t index = 0; index < current_num_tracks; index++) {
      pge_sprite_destroy(current_level->tracks[index].sprite);
    }

    pge_sprite_destroy(current_level->finish_line[0].sprite);
  }

  current_level = NULL;
}

static bool grect_overlaps_grect(GRect rect1, GRect rect2) {
  // Check if any of the four corner points of rect2 are in rect 1
  GPoint check_point = rect2.origin;
  if (grect_contains_point(&rect1, &check_point)) {
    return true;
  }
  check_point = GPoint(rect2.origin.x + rect2.size.w - 1, rect2.origin.y);
  if (grect_contains_point(&rect1, &check_point)) {
    return true;
  }
  check_point = GPoint(rect2.origin.x, rect2.origin.y + rect2.size.h - 1);
  if (grect_contains_point(&rect1, &check_point)) {
    return true;
  }
  check_point = GPoint(rect2.origin.x + rect2.size.w - 1, rect2.origin.y + rect2.size.h - 1);
  if (grect_contains_point(&rect1, &check_point)) {
    return true;
  }
  return false;
}

// Draw level based on the position of the car
void level_draw(GContext *ctx, GRect game_bounds) {
  if (current_level) {
  #ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, GColorJaegerGreen);
    GRect fill_rect = GRect(0 - game_bounds.origin.x, 0 - game_bounds.origin.y, SCREEN_RES_COLS, SCREEN_RES_ROWS);
    graphics_fill_rect(ctx, fill_rect, 0, GCornerNone);
  #endif

    // Only draw sprites that are on screen
    graphics_context_set_compositing_mode(ctx, GCompOpAssign);
    for (uint32_t index = 0; index < current_num_scenery; index++) {
      GRect sprite_bounds = pge_sprite_get_bounds(current_level->scenery[index].sprite);
      if (grect_overlaps_grect(fill_rect, sprite_bounds)) {
        pge_sprite_draw(current_level->scenery[index].sprite, ctx);
      }
    }
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    for (uint32_t index = 0; index < current_num_sprites; index++) {
      GRect sprite_bounds = pge_sprite_get_bounds(current_level->sprites[index].sprite);
      if (grect_overlaps_grect(fill_rect, sprite_bounds)) {
        pge_sprite_draw(current_level->sprites[index].sprite, ctx);
      }
    }
    for (uint32_t index = 0; index < current_num_walls; index++) {
      GRect sprite_bounds = pge_sprite_get_bounds(current_level->walls[index].sprite);
      if (grect_overlaps_grect(fill_rect, sprite_bounds)) {
        pge_sprite_draw(current_level->walls[index].sprite, ctx);
      }
    }
    for (uint32_t index = 0; index < current_num_tracks; index++) {
      GRect sprite_bounds = pge_sprite_get_bounds(current_level->tracks[index].sprite);
      if (grect_overlaps_grect(fill_rect, sprite_bounds)) {
        pge_sprite_draw(current_level->tracks[index].sprite, ctx);
      }
    }

    pge_sprite_draw(current_level->finish_line[0].sprite, ctx);
  }
}

#define CAR_BOUNDARY_OFFSET 3 // This is to offset from the corner point so the corner isn't calculated in the collision
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

  // Check center point of each wall of the car to see if it overlaps with the bounds of any wall - if any collide, mask out
  // the appropriate direction of collision
  if (current_level) {
    // Check walls
    for (uint32_t index = 0; index < current_num_walls; index++) {
      GRect wall_bounds = pge_sprite_get_bounds(current_level->walls[index].sprite);
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
    // Check other sprites
    for (uint32_t index = 0; index < current_num_sprites; index++) {
      GRect sprite_bounds = pge_sprite_get_bounds(current_level->sprites[index].sprite);
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
  // Check center point of each wall of the car to see if it overlaps with the bounds of the opponent car - if any collide, mask out
  // the appropriate direction of collision
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

void update_car_angle_opp(Car* car_ptr) {
  if (!car_ptr->moving) {
    return;
  }

  // Get car's center point
  GRect car_bounds = pge_sprite_get_bounds(car_ptr->sprite_color);
  GPoint car_origin = pge_sprite_get_position(car_ptr->sprite_color);
  GPoint car_center = GPoint(car_origin.x + (car_bounds.size.w / 2), 
                             car_origin.y + (car_bounds.size.h / 2));

  // Get car's current track point index
  int track_point_index = car_ptr->track_point_index;
  GPoint current_track_point = current_level->track_points0[track_point_index];

  // If car overlaps current track pointer index, update to next track point index
  if (pge_collision_point_rectangle(&current_track_point, &car_bounds)) {
    track_point_index = (track_point_index + 1) % current_num_track_points0;
    car_ptr->track_point_index = track_point_index;
    current_track_point = current_level->track_points0[track_point_index];
  }

  // Determine angle between car center point and current track pointer index
  int16_t dx = car_center.x - current_track_point.x;
  int16_t dy = car_center.y - current_track_point.y;
  car_ptr->angle = ((360 * atan2_lookup(dy, dx)) / TRIG_MAX_ANGLE) - 90; // subtract 90 to get correct angle

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "--opp: %d %d %d", dx, dy, car_ptr->angle);

  pge_sprite_set_rotation(car_ptr->sprite_color, DEG_TO_TRIG_ANGLE(car_ptr->angle));
}

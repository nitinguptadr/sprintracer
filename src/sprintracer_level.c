#include <pebble.h>

#include "sprintracer_common.h"
#include "pge/additional/pge_sprite.h"
#include "pge/additional/pge_collision.h"
#include "resources/images/level0_sprites.h"
#include "resources/images/level1_sprites.h"
#include "resources/images/level2_sprites.h"

static LevelNumId s_current_level = 0;
static LevelSpriteDetails *current_level = NULL;
static uint32_t current_num_scenery = 0;
static uint32_t current_num_sprites = 0;
static uint32_t current_num_walls = 0;
static uint32_t current_num_tracks = 0;
static uint32_t current_num_track_points0 = 0;
static uint32_t current_num_checkpoints = 0;
static uint32_t current_num_car_locations = 0;

typedef enum {
  CANNONBALL_STATE_NONE,    // No cannonball on screen
  CANNONBALL_STATE_PRESENT, // cannonball on screen, not picked up by a car
  CANNONBALL_STATE_LOADED,  // cannonball picked up by a car
  CANNONBALL_STATE_SHOT,    // cannonball shot by a car
} CannonballState;

// Battle racing info
static CannonballState s_cannonball_state = CANNONBALL_STATE_NONE;
static GPoint s_cannonball_offset;
static PGESprite *s_cannonball_sprite;
static int32_t s_cannonball_angle = 0;
static AppTimer *s_cannonball_timer;

static void cannonball_timer_handler(void *context) {
  Car *car_ptr = (Car *)context;
  car_ptr->moving = true;
}

static void cannonball_reset_pos() {
  GPoint cannonball_pos = current_level->car_locations[0]; // Place at the same location of where the car started
  s_cannonball_state = CANNONBALL_STATE_NONE;
  s_cannonball_angle = 0;
  pge_sprite_set_position(s_cannonball_sprite, cannonball_pos);
  pge_sprite_set_rotation(s_cannonball_sprite, DEG_TO_TRIG_ANGLE(s_cannonball_angle));
}

// Show the cannonball on screen
static void cannonball_show() {
  if (!current_level) {
    return;
  }

  cannonball_reset_pos();
  s_cannonball_state = CANNONBALL_STATE_PRESENT;

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Showing cannon %d %d", s_cannonball_sprite->position.x, s_cannonball_sprite->position.y);
}

#define CANNONBALL_SPEED_DEFAULT 6
// Fire the cannonball if in appropriate state
void cannonball_fire(Car *car_ptr) {
  if (s_cannonball_state != CANNONBALL_STATE_LOADED) {
    return;
  }

  // Set initial cannonball position based on car's position and angle - center the cannon ball
  int32_t angle = DEG_TO_TRIG_ANGLE(car_ptr->angle);
  float sin_value = ((float)sin_lookup(angle)) / TRIG_MAX_RATIO;
  float cos_value = ((float)cos_lookup(angle)) / TRIG_MAX_RATIO;
  float x_direction = CANNONBALL_SPEED_DEFAULT*sin_value;
  float y_direction = -CANNONBALL_SPEED_DEFAULT*cos_value;

  s_cannonball_offset.x = (int16_t)x_direction;
  s_cannonball_offset.y = (int16_t)y_direction;

  GRect car_bounds = pge_sprite_get_bounds(car_ptr->sprite_color);
  car_bounds.origin.x += car_bounds.size.w / 2 + (2 * s_cannonball_offset.x);
  car_bounds.origin.y += car_bounds.size.h / 2 + (2 * s_cannonball_offset.y);
  pge_sprite_set_position(s_cannonball_sprite, car_bounds.origin);
  s_cannonball_state = CANNONBALL_STATE_SHOT;

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Firing cannon %d %d", s_cannonball_sprite->position.x, s_cannonball_sprite->position.y);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Firing cannon with offset %d %d", s_cannonball_offset.x, s_cannonball_offset.y);
}

// Update current position of the cannonball
void cannonball_update(Car *user, Car *opp1, Car *opp2, Car *opp3) {
  if ((s_cannonball_state == CANNONBALL_STATE_NONE) ||
      (s_cannonball_state == CANNONBALL_STATE_LOADED)) {
    return;
  }

  // Only update position of cannonball if shot or present
  GRect cannon_bounds = pge_sprite_get_bounds(s_cannonball_sprite);
  bool overlap_user = (level_collision_cars(cannon_bounds, user) != DIRECTION_ALL);

  if (s_cannonball_state == CANNONBALL_STATE_PRESENT) {
    // Load up the cannonball
    if (overlap_user) {
      s_cannonball_state = CANNONBALL_STATE_LOADED;
    }
  } else if (s_cannonball_state == CANNONBALL_STATE_SHOT) {
    bool overlap_opp1 = (level_collision_cars(cannon_bounds, opp1) != DIRECTION_ALL);
    bool overlap_opp2 = (level_collision_cars(cannon_bounds, opp2) != DIRECTION_ALL);
    bool overlap_opp3 = (level_collision_cars(cannon_bounds, opp3) != DIRECTION_ALL);
    GPoint current_pos = pge_sprite_get_position(s_cannonball_sprite);

    //APP_LOG(APP_LOG_LEVEL_DEBUG, "Cannonball bounds origin %d %d", cannon_bounds.origin.x, cannon_bounds.origin.y);
    // Check if its colliding with anything - if not, then update its position
    // If colliding with an opponent car, disable that car
    if (level_collision_walls(cannon_bounds) != DIRECTION_ALL) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Cannonball hit walls %d %d", current_pos.x, current_pos.y);
      s_cannonball_state = CANNONBALL_STATE_NONE;
    } else if (overlap_opp1) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Cannonball hit opp1 %d %d", current_pos.x, current_pos.y);
      s_cannonball_state = CANNONBALL_STATE_NONE;
      opp1->moving = false;
      s_cannonball_timer = app_timer_register(1000, cannonball_timer_handler, (void*)opp1);
    } else if (overlap_opp2) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Cannonball hit opp2 %d %d", current_pos.x, current_pos.y);
      s_cannonball_state = CANNONBALL_STATE_NONE;
      s_cannonball_timer = app_timer_register(1000, cannonball_timer_handler, (void*)opp2);
      opp2->moving = false;
    } else if (overlap_opp3) {
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Cannonball hit opp3  %d %d", current_pos.x, current_pos.y);
      s_cannonball_state = CANNONBALL_STATE_NONE;
      s_cannonball_timer = app_timer_register(1000, cannonball_timer_handler, (void*)opp3);
      opp3->moving = false;
    } else {
      current_pos.x += s_cannonball_offset.x;
      current_pos.y += s_cannonball_offset.y;
      pge_sprite_set_position(s_cannonball_sprite, current_pos);
      s_cannonball_angle = (s_cannonball_angle + 30) % 360;   // simulate some rotatation
      pge_sprite_set_rotation(s_cannonball_sprite, DEG_TO_TRIG_ANGLE(s_cannonball_angle));
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Cannonball position %d %d", current_pos.x, current_pos.y);
    }
  }
}

// Initializes the current level - allocate all the memory for sprites
void level_initialize(Layer *game_layer, LevelNumId level) {
  switch(level) {
    case LEVEL0_ID:
      layer_set_frame(game_layer, LEVEL0_BOUNDS);
      current_level = &level0_details;
      current_num_scenery = sizeof(level0_scenery) / sizeof(LevelSpriteLocation);
      current_num_sprites = sizeof(level0_sprites) / sizeof(LevelSpriteLocation);
      current_num_walls = sizeof(level0_walls) / sizeof(LevelSpriteLocation);
      current_num_tracks = sizeof(level0_tracks) / sizeof(LevelSpriteLocation);
      current_num_checkpoints = sizeof(level0_checkpoints) / sizeof(LevelSpriteLocation);
      current_num_track_points0 = sizeof(level0_track_points0) / sizeof(GPoint);
      current_num_car_locations = sizeof(level0_car_locations) / sizeof(GPoint);
      break;
    case LEVEL1_ID:
      layer_set_frame(game_layer, LEVEL1_BOUNDS);
      current_level = &level1_details;
      current_num_scenery = sizeof(level1_scenery) / sizeof(LevelSpriteLocation);
      current_num_sprites = sizeof(level1_sprites) / sizeof(LevelSpriteLocation);
      current_num_walls = sizeof(level1_walls) / sizeof(LevelSpriteLocation);
      current_num_tracks = sizeof(level1_tracks) / sizeof(LevelSpriteLocation);
      current_num_checkpoints = sizeof(level1_checkpoints) / sizeof(LevelSpriteLocation);
      current_num_track_points0 = sizeof(level1_track_points0) / sizeof(GPoint);
      current_num_car_locations = sizeof(level1_car_locations) / sizeof(GPoint);
      break;
    case LEVEL2_ID:
      layer_set_frame(game_layer, LEVEL2_BOUNDS);
      current_level = &level2_details;
      current_num_scenery = sizeof(level2_scenery) / sizeof(LevelSpriteLocation);
      current_num_sprites = sizeof(level2_sprites) / sizeof(LevelSpriteLocation);
      current_num_walls = sizeof(level2_walls) / sizeof(LevelSpriteLocation);
      current_num_tracks = sizeof(level2_tracks) / sizeof(LevelSpriteLocation);
      current_num_checkpoints = sizeof(level2_checkpoints) / sizeof(LevelSpriteLocation);
      current_num_track_points0 = sizeof(level2_track_points0) / sizeof(GPoint);
      current_num_car_locations = sizeof(level2_car_locations) / sizeof(GPoint);
      break;
    default:
      current_level = NULL;
      break;
  }

  if (current_level) {
    // Initialize sprites for all elements
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
    for (uint32_t index = 0; index < current_num_checkpoints; index++) {
      current_level->checkpoints[index].sprite = pge_sprite_create(current_level->checkpoints[index].offset, current_level->checkpoints[index].resource_id);
    }

    // Create finish group items
    current_level->finish_group->finish_line.sprite = pge_sprite_create(current_level->finish_group->finish_line.offset, current_level->finish_group->finish_line.resource_id);
    current_level->finish_group->finish_box_1.sprite = pge_sprite_create(current_level->finish_group->finish_box_1.offset, current_level->finish_group->finish_box_1.resource_id);
    current_level->finish_group->finish_box_2.sprite = pge_sprite_create(current_level->finish_group->finish_box_2.offset, current_level->finish_group->finish_box_2.resource_id);
    current_level->finish_group->finish_box_3.sprite = pge_sprite_create(current_level->finish_group->finish_box_3.offset, current_level->finish_group->finish_box_3.resource_id);
    current_level->finish_group->finish_box_4.sprite = pge_sprite_create(current_level->finish_group->finish_box_4.offset, current_level->finish_group->finish_box_4.resource_id);
    current_level->finish_group->light_signal.sprite = pge_sprite_create(current_level->finish_group->light_signal.offset, current_level->finish_group->light_signal.resource_id);

    // Initialize the finish order of all cars
    for (int pos = 0; pos < NUM_CARS_TOTAL; pos++) {
      current_level->finish_order[pos] = NULL;
    }

    // Initialize sprite for cannon ball to be located in the center of the last track piece
    s_cannonball_sprite = pge_sprite_create(GPointZero, RESOURCE_ID_CANNONBALL);
    cannonball_reset_pos();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Level %d initialized", level);
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Invalid level %d. Could not initialize", level);
  }
}

// Deinitializes the current level - deallocate all the memory
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
    for (uint32_t index = 0; index < current_num_checkpoints; index++) {
      pge_sprite_destroy(current_level->checkpoints[index].sprite);
    }

    pge_sprite_destroy(current_level->finish_group->finish_line.sprite);
    pge_sprite_destroy(current_level->finish_group->finish_box_1.sprite);
    pge_sprite_destroy(current_level->finish_group->finish_box_2.sprite);
    pge_sprite_destroy(current_level->finish_group->finish_box_3.sprite);
    pge_sprite_destroy(current_level->finish_group->finish_box_4.sprite);
    pge_sprite_destroy(current_level->finish_group->light_signal.sprite);

    pge_sprite_destroy(s_cannonball_sprite);
  }

  current_level = NULL;
}

// Sets the current level
void level_set_current(LevelNumId level) {
  s_current_level = level;
}

// Returns the current level
LevelNumId level_get_current() {
  return s_current_level;
}

// Returns whether two rectangles overlap or not
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
  if (!current_level) {
    return;
  }
#ifdef PBL_COLOR
  graphics_context_set_fill_color(ctx, GColorJaegerGreen);
  GRect fill_rect = GRect(0 - game_bounds.origin.x, 0 - game_bounds.origin.y, SCREEN_RES_COLS, SCREEN_RES_ROWS);
  graphics_fill_rect(ctx, fill_rect, 0, GCornerNone);
#endif

  // Only draw sprites that are on screen
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
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

  // Draw finish line
  GRect sprite_bounds = pge_sprite_get_bounds(current_level->finish_group->finish_line.sprite);
  if (grect_overlaps_grect(fill_rect, sprite_bounds)) {
    pge_sprite_draw(current_level->finish_group->finish_line.sprite, ctx);
  }

  // Draw finish boxes and street signal
  graphics_context_set_compositing_mode(ctx, GCompOpAssign);
  sprite_bounds = pge_sprite_get_bounds(current_level->finish_group->finish_box_1.sprite);
  if (grect_overlaps_grect(fill_rect, sprite_bounds)) {
    pge_sprite_draw(current_level->finish_group->finish_box_1.sprite, ctx);
  }
  sprite_bounds = pge_sprite_get_bounds(current_level->finish_group->finish_box_2.sprite);
  if (grect_overlaps_grect(fill_rect, sprite_bounds)) {
    pge_sprite_draw(current_level->finish_group->finish_box_2.sprite, ctx);
  }
  sprite_bounds = pge_sprite_get_bounds(current_level->finish_group->finish_box_3.sprite);
  if (grect_overlaps_grect(fill_rect, sprite_bounds)) {
    pge_sprite_draw(current_level->finish_group->finish_box_3.sprite, ctx);
  }
  sprite_bounds = pge_sprite_get_bounds(current_level->finish_group->finish_box_4.sprite);
  if (grect_overlaps_grect(fill_rect, sprite_bounds)) {
    pge_sprite_draw(current_level->finish_group->finish_box_4.sprite, ctx);
  }
  sprite_bounds = pge_sprite_get_bounds(current_level->finish_group->light_signal.sprite);
  if (grect_overlaps_grect(fill_rect, sprite_bounds)) {
    pge_sprite_draw(current_level->finish_group->light_signal.sprite, ctx);
  }

  // Draw cannonball based on state
  if ((s_cannonball_state == CANNONBALL_STATE_PRESENT) ||
      (s_cannonball_state == CANNONBALL_STATE_SHOT)) {
    pge_sprite_draw(s_cannonball_sprite, ctx);
  }
}

// Check if a car collides with any walls or other sprites and return the allowable directions
// i.e. up, down, left, and/or right
#define CAR_BOUNDARY_OFFSET 3 // This is to offset from the corner point so the corner isn't calculated in the collision
uint8_t level_collision_walls(GRect car_bounds) {
  GLine top =    { GPoint(car_bounds.origin.x + CAR_BOUNDARY_OFFSET, car_bounds.origin.y),
                   GPoint(car_bounds.origin.x + car_bounds.size.w - CAR_BOUNDARY_OFFSET, car_bounds.origin.y) };
  GLine left =   { GPoint(car_bounds.origin.x, car_bounds.origin.y + CAR_BOUNDARY_OFFSET),
                   GPoint(car_bounds.origin.x, car_bounds.origin.y + car_bounds.size.h - CAR_BOUNDARY_OFFSET) };
  GLine right =  { GPoint(car_bounds.origin.x + car_bounds.size.w, car_bounds.origin.y + CAR_BOUNDARY_OFFSET),
                   GPoint(car_bounds.origin.x + car_bounds.size.w, car_bounds.origin.y + car_bounds.size.h - CAR_BOUNDARY_OFFSET) };
  GLine bottom = { GPoint(car_bounds.origin.x + CAR_BOUNDARY_OFFSET, car_bounds.origin.y + car_bounds.size.h),
                   GPoint(car_bounds.origin.x + car_bounds.size.w - CAR_BOUNDARY_OFFSET, car_bounds.origin.y + car_bounds.size.h) };

  GPoint top_center = GPoint((top.p1.x + top.p2.x) / 2, (top.p1.y + top.p2.y) / 2);
  GPoint left_center = GPoint((left.p1.x + left.p2.x) / 2, (left.p1.y + left.p2.y) / 2);
  GPoint right_center = GPoint((right.p1.x + right.p2.x) / 2, (right.p1.y + right.p2.y) / 2);
  GPoint bottom_center = GPoint((bottom.p1.x + bottom.p2.x) / 2, (bottom.p1.y + bottom.p2.y) / 2);

  uint8_t allowable_directions = DIRECTION_ALL;

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Car Bounds %d %d %d %d", car_bounds.origin.x, car_bounds.origin.y, car_bounds.size.w, car_bounds.size.h);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "T: %d %d B: %d %d", top_center.x, top_center.y, bottom_center.x, bottom_center.y);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "L: %d %d R: %d %d", left_center.x, left_center.y, right_center.x, right_center.y);

  // Check center point of each wall of the car to see if it overlaps with the bounds of any wall - if any collide, mask out
  // the appropriate direction of collision
  if (current_level) {
    // Check walls
    for (uint32_t index = 0; index < current_num_walls; index++) {
      GRect wall_bounds = pge_sprite_get_bounds(current_level->walls[index].sprite);
      if (pge_collision_line_rectangle(&top, &wall_bounds) || pge_collision_point_rectangle(&top_center, &wall_bounds)) {
        allowable_directions &= ~((uint8_t)DIRECTION_UP);
        if (pge_collision_line_rectangle(&top, &wall_bounds)) {
          //APP_LOG(APP_LOG_LEVEL_DEBUG, "Wall Collision U %lu", index);
        }
        if (pge_collision_point_rectangle(&top_center, &wall_bounds)) {
          //APP_LOG(APP_LOG_LEVEL_DEBUG, "Wall Collision U %d %d %d %d", wall_bounds.origin.x, wall_bounds.origin.y, wall_bounds.size.w, wall_bounds.size.h);
        }
      }
      if (pge_collision_line_rectangle(&left, &wall_bounds) || pge_collision_point_rectangle(&left_center, &wall_bounds)) {
        allowable_directions &= ~((uint8_t)DIRECTION_LEFT);
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Wall Collision L %lu", index);
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Wall Collision L %d %d %d %d", wall_bounds.origin.x, wall_bounds.origin.y, wall_bounds.size.w, wall_bounds.size.h);
      }
      if (pge_collision_line_rectangle(&right, &wall_bounds) || pge_collision_point_rectangle(&right_center, &wall_bounds)) {
        allowable_directions &= ~((uint8_t)DIRECTION_RIGHT);
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Wall Collision R %lu", index);
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Wall Collision R %d %d %d %d", wall_bounds.origin.x, wall_bounds.origin.y, wall_bounds.size.w, wall_bounds.size.h);
      }
      if (pge_collision_line_rectangle(&bottom, &wall_bounds) || pge_collision_point_rectangle(&bottom_center, &wall_bounds)) {
        allowable_directions &= ~((uint8_t)DIRECTION_DOWN);
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Wall Collision D %lu", index);
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Wall Collision D %d %d %d %d", wall_bounds.origin.x, wall_bounds.origin.y, wall_bounds.size.w, wall_bounds.size.h);
      }
    }
    // Check other sprites
    for (uint32_t index = 0; index < current_num_sprites; index++) {
      GRect sprite_bounds = pge_sprite_get_bounds(current_level->sprites[index].sprite);
      if (pge_collision_line_rectangle(&top, &sprite_bounds) || pge_collision_point_rectangle(&top_center, &sprite_bounds)) {
        allowable_directions &= ~((uint8_t)DIRECTION_UP);
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision U %lu", index);
      }
      if (pge_collision_line_rectangle(&left, &sprite_bounds) || pge_collision_point_rectangle(&left_center, &sprite_bounds)) {
        allowable_directions &= ~((uint8_t)DIRECTION_LEFT);
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision L %lu", index);
      }
      if (pge_collision_line_rectangle(&right, &sprite_bounds) || pge_collision_point_rectangle(&right_center, &sprite_bounds)) {
        allowable_directions &= ~((uint8_t)DIRECTION_RIGHT);
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision R %lu", index);
      }
      if (pge_collision_line_rectangle(&bottom, &sprite_bounds) || pge_collision_point_rectangle(&bottom_center, &sprite_bounds)) {
        allowable_directions &= ~((uint8_t)DIRECTION_DOWN);
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision D %lu", index);
      }
    }

    // Check finish group
    GRect sprite_bounds = pge_sprite_get_bounds(current_level->finish_group->finish_box_1.sprite);
    if (pge_collision_line_rectangle(&top, &sprite_bounds) || pge_collision_point_rectangle(&top_center, &sprite_bounds)) {
      allowable_directions &= ~((uint8_t)DIRECTION_UP);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision U %lu", index);
    }
    if (pge_collision_line_rectangle(&left, &sprite_bounds) || pge_collision_point_rectangle(&left_center, &sprite_bounds)) {
      allowable_directions &= ~((uint8_t)DIRECTION_LEFT);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision L %lu", index);
    }
    if (pge_collision_line_rectangle(&right, &sprite_bounds) || pge_collision_point_rectangle(&right_center, &sprite_bounds)) {
      allowable_directions &= ~((uint8_t)DIRECTION_RIGHT);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision R %lu", index);
    }
    if (pge_collision_line_rectangle(&bottom, &sprite_bounds) || pge_collision_point_rectangle(&bottom_center, &sprite_bounds)) {
      allowable_directions &= ~((uint8_t)DIRECTION_DOWN);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision D %lu", index);
    }

    sprite_bounds = pge_sprite_get_bounds(current_level->finish_group->finish_box_2.sprite);
    if (pge_collision_line_rectangle(&top, &sprite_bounds) || pge_collision_point_rectangle(&top_center, &sprite_bounds)) {
      allowable_directions &= ~((uint8_t)DIRECTION_UP);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision U %lu", index);
    }
    if (pge_collision_line_rectangle(&left, &sprite_bounds) || pge_collision_point_rectangle(&left_center, &sprite_bounds)) {
      allowable_directions &= ~((uint8_t)DIRECTION_LEFT);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision L %lu", index);
    }
    if (pge_collision_line_rectangle(&right, &sprite_bounds) || pge_collision_point_rectangle(&right_center, &sprite_bounds)) {
      allowable_directions &= ~((uint8_t)DIRECTION_RIGHT);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision R %lu", index);
    }
    if (pge_collision_line_rectangle(&bottom, &sprite_bounds) || pge_collision_point_rectangle(&bottom_center, &sprite_bounds)) {
      allowable_directions &= ~((uint8_t)DIRECTION_DOWN);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision D %lu", index);
    }

    sprite_bounds = pge_sprite_get_bounds(current_level->finish_group->finish_box_3.sprite);
    if (pge_collision_line_rectangle(&top, &sprite_bounds) || pge_collision_point_rectangle(&top_center, &sprite_bounds)) {
      allowable_directions &= ~((uint8_t)DIRECTION_UP);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision U %lu", index);
    }
    if (pge_collision_line_rectangle(&left, &sprite_bounds) || pge_collision_point_rectangle(&left_center, &sprite_bounds)) {
      allowable_directions &= ~((uint8_t)DIRECTION_LEFT);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision L %lu", index);
    }
    if (pge_collision_line_rectangle(&right, &sprite_bounds) || pge_collision_point_rectangle(&right_center, &sprite_bounds)) {
      allowable_directions &= ~((uint8_t)DIRECTION_RIGHT);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision R %lu", index);
    }
    if (pge_collision_line_rectangle(&bottom, &sprite_bounds) || pge_collision_point_rectangle(&bottom_center, &sprite_bounds)) {
      allowable_directions &= ~((uint8_t)DIRECTION_DOWN);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision D %lu", index);
    }

    sprite_bounds = pge_sprite_get_bounds(current_level->finish_group->finish_box_4.sprite);
    if (pge_collision_line_rectangle(&top, &sprite_bounds) || pge_collision_point_rectangle(&top_center, &sprite_bounds)) {
      allowable_directions &= ~((uint8_t)DIRECTION_UP);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision U %lu", index);
    }
    if (pge_collision_line_rectangle(&left, &sprite_bounds) || pge_collision_point_rectangle(&left_center, &sprite_bounds)) {
      allowable_directions &= ~((uint8_t)DIRECTION_LEFT);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision L %lu", index);
    }
    if (pge_collision_line_rectangle(&right, &sprite_bounds) || pge_collision_point_rectangle(&right_center, &sprite_bounds)) {
      allowable_directions &= ~((uint8_t)DIRECTION_RIGHT);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision R %lu", index);
    }
    if (pge_collision_line_rectangle(&bottom, &sprite_bounds) || pge_collision_point_rectangle(&bottom_center, &sprite_bounds)) {
      allowable_directions &= ~((uint8_t)DIRECTION_DOWN);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision D %lu", index);
    }

    sprite_bounds = pge_sprite_get_bounds(current_level->finish_group->light_signal.sprite);
    if (pge_collision_line_rectangle(&top, &sprite_bounds) || pge_collision_point_rectangle(&top_center, &sprite_bounds)) {
      allowable_directions &= ~((uint8_t)DIRECTION_UP);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision U %lu", index);
    }
    if (pge_collision_line_rectangle(&left, &sprite_bounds) || pge_collision_point_rectangle(&left_center, &sprite_bounds)) {
      allowable_directions &= ~((uint8_t)DIRECTION_LEFT);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision L %lu", index);
    }
    if (pge_collision_line_rectangle(&right, &sprite_bounds) || pge_collision_point_rectangle(&right_center, &sprite_bounds)) {
      allowable_directions &= ~((uint8_t)DIRECTION_RIGHT);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision R %lu", index);
    }
    if (pge_collision_line_rectangle(&bottom, &sprite_bounds) || pge_collision_point_rectangle(&bottom_center, &sprite_bounds)) {
      allowable_directions &= ~((uint8_t)DIRECTION_DOWN);
      //APP_LOG(APP_LOG_LEVEL_DEBUG, "Sprite Collision D %lu", index);
    }

  }

  return allowable_directions;
}

// Check if a car collides with another car and returns the allowable directions of movement
// i.e. up, down, left, and/or right
uint8_t level_collision_cars(GRect car_bounds, Car *car_ptr) {
  uint8_t allowable_directions = DIRECTION_ALL;

  if (!car_ptr->moving) {
    return allowable_directions;
  }

  GRect car_bounds_opponent = pge_sprite_get_bounds(car_ptr->sprite_color);
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

// Updates the current checkpoint of the user to the next checkpoint based on whether the car
// collides with the current checkpoint
void update_checkpoints(Car *car_ptr) {
  if (car_ptr->current_checkpoint >= current_num_checkpoints) {
    return;
  }

  GRect current_checkpoint_bounds = pge_sprite_get_bounds(current_level->checkpoints[car_ptr->current_checkpoint].sprite);
  GRect car_bounds = pge_sprite_get_bounds(car_ptr->sprite_color);
  if (pge_collision_rectangle_rectangle(&current_checkpoint_bounds, &car_bounds)) {
    car_ptr->current_checkpoint++;
  }
}

// Updates a car's placement (i.e. 1st, 2nd, 3rd place). Each time this API is called, it updates
// the placement of the car to the next available slot and also sets which finish box the car
// should be placed in after finishing the race.
void set_placement_position(Car *car_ptr) {
  int placement = car_ptr->placement;
  if (placement == CAR_PLACEMENT_UNSET) {
    return;
  }

  // Move car to finish box
  PGESprite *finish_box = NULL;
  if (placement == 1) {
    finish_box = current_level->finish_group->finish_box_1.sprite;
  } else if (placement == 2) {
    finish_box = current_level->finish_group->finish_box_2.sprite;
  } else if (placement == 3) {
    finish_box = current_level->finish_group->finish_box_3.sprite;
  } else if (placement == 4) {
    finish_box = current_level->finish_group->finish_box_4.sprite;
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "ERROR: update_placement - Should not reach here\n");
    return; // Should never reach here
  }

  // Get position of finish box and center car within it
  GPoint finish_box_pos = pge_sprite_get_position(finish_box);
  GRect finish_box_bounds = pge_sprite_get_bounds(finish_box);
  GRect car_bounds = pge_sprite_get_bounds(car_ptr->sprite_color);
  int x_offset = (finish_box_bounds.size.w - car_bounds.size.w) / 2;
  int y_offset = (finish_box_bounds.size.h - car_bounds.size.h) / 2;
  finish_box_pos.x += x_offset;
  finish_box_pos.y += y_offset;

  // Update car to be in the correct position
  car_ptr->prev_angle = 0;
  car_ptr->angle = 0;
  pge_sprite_set_rotation(car_ptr->sprite_color, car_ptr->angle);

  car_ptr->pos_x = finish_box_pos.x;
  car_ptr->pos_y = finish_box_pos.y;

  pge_sprite_set_position(car_ptr->sprite_color, finish_box_pos);
}

// Only call this when crossing finish line
void update_placements(Car *car_ptr) {
  for (int pos = 0; pos < NUM_CARS_TOTAL; pos++) {
    // Find next available position slot
    if (current_level->finish_order[pos] == NULL) {
      current_level->finish_order[pos] = car_ptr;
      car_ptr->placement = pos + 1;
      update_tournament_score(car_ptr);
      break;
    }
  }
}

// Update the current angle of an opponent car. This is based on the relative positions of where
// the car is and where the current track point is.
void update_car_angle_opp(Car* car_ptr) {
  if (!car_ptr->moving) {
    return;
  }

  // Get car's center point
  GRect car_bounds = pge_sprite_get_bounds(car_ptr->sprite_color);
  GPoint car_origin = pge_sprite_get_position(car_ptr->sprite_color);
  GPoint car_center = GPoint(car_origin.x + (car_bounds.size.w / 2), 
                             car_origin.y + (car_bounds.size.h / 2));

  // Determine angle between car center point and current track pointer index
  unsigned int track_point_index = car_ptr->track_point_index;
  GPoint current_track_point = current_level->track_points0[track_point_index];
  int track_point_offset = (track_point_index == current_num_track_points0 - 1) ? 
                           0 : car_ptr->track_point_offset;
  current_track_point.x += track_point_offset;
  current_track_point.y += track_point_offset;

  int16_t dx = car_center.x - current_track_point.x;
  int16_t dy = car_center.y - current_track_point.y;
  int angle = ((360 * atan2_lookup(dy, dx)) / TRIG_MAX_ANGLE) - 90; // subtract 90 to get correct angle
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "angle: %d", angle);

#if 0 // TODO: Causes cars to go wild
  // Update the current angle of the car by a maximum amount
  if ((angle > car_ptr->angle) && ((angle - car_ptr->angle) > OPP_ANGLE_CHANGE)) {
    car_ptr->angle += OPP_ANGLE_CHANGE;
  } else if ((angle < car_ptr->angle) && ((car_ptr->angle - angle) > OPP_ANGLE_CHANGE)) {
    car_ptr->angle -= OPP_ANGLE_CHANGE;
  } else {
  }
#endif
  car_ptr->angle = angle;
    
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "--opp: %d %d %d", dx, dy, car_ptr->angle);
  
  pge_sprite_set_rotation(car_ptr->sprite_color, DEG_TO_TRIG_ANGLE(car_ptr->angle));
}

// Updates an opponent's track point to the next one if it overlaps with the current track point
void update_track_point(Car *car_ptr) {
  GRect car_bounds = pge_sprite_get_bounds(car_ptr->sprite_color);

  // Get car's current track point index
  unsigned int track_point_index = car_ptr->track_point_index;
  int track_point_offset = (track_point_index == current_num_track_points0 - 1) ? 
                           0 : car_ptr->track_point_offset;
  GPoint current_track_point = current_level->track_points0[track_point_index];

  // If car overlaps current track pointer index, update to next track point index
  // Create a rectangle with current point as center
  GRect current_rect = GRect(current_track_point.x - TRACKPOINT_RADIUS + track_point_offset,
                             current_track_point.y - TRACKPOINT_RADIUS + track_point_offset,
                             2 * TRACKPOINT_RADIUS,
                             2 * TRACKPOINT_RADIUS);
  if (pge_collision_rectangle_rectangle(&current_rect, &car_bounds)) {
    track_point_index = (track_point_index + 1) % current_num_track_points0;
    // Update lap number if crossing finish line
    if (car_ptr->track_point_index > track_point_index) {
      car_ptr->lap++;
    }
    car_ptr->track_point_index = track_point_index;
    current_track_point = current_level->track_points0[track_point_index];
  }
}

// Returns true if car is colliding with the finish line
static bool crossing_finish_line(Car *car_ptr) {
  GRect car_bounds = pge_sprite_get_bounds(car_ptr->sprite_color);
  GRect finish_line_rect = pge_sprite_get_bounds(current_level->finish_group->finish_line.sprite);
  return pge_collision_rectangle_rectangle(&car_bounds, &finish_line_rect);
}

// Updates the lap of the user car if it crosses the finish line and the current checkpoint is
// greater than or equal to the last checkpoint - doesn't update if checkpoint is less or 0.
void update_user_lap(Car *car_ptr) {
  bool current_state = crossing_finish_line(car_ptr);
  bool crossing_finish = ((current_state != car_ptr->crossing_finish) && (car_ptr->crossing_finish == false));
  if (crossing_finish &&
      (car_ptr->current_checkpoint >= current_num_checkpoints)) {
    car_ptr->lap++;
    car_ptr->current_checkpoint = 0; // Reset checkpoints for next lap

    if ((car_ptr->lap <= NUM_LAPS_DEFAULT) && (s_cannonball_state == CANNONBALL_STATE_NONE)) {
      cannonball_show();
    }
  } else if (crossing_finish && (car_ptr->lap == 1)) {
    if ((car_ptr->lap <= NUM_LAPS_DEFAULT) && (s_cannonball_state == CANNONBALL_STATE_NONE)) {
      cannonball_show();
    }
  }
  car_ptr->crossing_finish = current_state;
}

// Update the state of the light signal based on the countdown timer
void update_signal(int countdown) {
  if (countdown == 3) {
    pge_sprite_set_anim_frame(current_level->finish_group->light_signal.sprite, RESOURCE_ID_LIGHT_SIGNAL_NONE);
  } else if (countdown == 2) {
    pge_sprite_set_anim_frame(current_level->finish_group->light_signal.sprite, RESOURCE_ID_LIGHT_SIGNAL_RED);
  } else if (countdown == 1) {
    pge_sprite_set_anim_frame(current_level->finish_group->light_signal.sprite, RESOURCE_ID_LIGHT_SIGNAL_YELLOW);
  } else if (countdown == 0) {
    pge_sprite_set_anim_frame(current_level->finish_group->light_signal.sprite, RESOURCE_ID_LIGHT_SIGNAL_GREEN);
  } else {
    pge_sprite_set_anim_frame(current_level->finish_group->light_signal.sprite, RESOURCE_ID_LIGHT_SIGNAL_ALL);
  }
}

// Get the starting location of a car for the current level
GPoint get_starting_location(int index) {
  return current_level->car_locations[index];
}

// Get the starting angle of a car for the current level
int get_starting_angle() {
  return current_level->car_starting_angle;
}

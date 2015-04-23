#pragma once

#include "pebble_sprint_level.h"

#define LEVEL0_BOUNDS GRect(0, 0, SCREEN_RES_COLS*4, SCREEN_RES_ROWS*4)

#define LEVEL0_CAR_STARTING_ANGLE -90

// Used for sprite collision - drawn in order of index in array
static LevelSpriteLocation level0_sprites[] = {
  { RESOURCE_ID_VERT_TREE_35, {245, 10}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {225, 20}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {250, 30}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {15, 35}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {40, 10}, NULL },
  { RESOURCE_ID_VERT_BUSH_35, {20, 15}, NULL },
  { RESOURCE_ID_VERT_BUSH_35, {35, 35}, NULL },
  { RESOURCE_ID_VERT_BUSH_35, {10, 55}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {15, 285}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {10, 265}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {30, 290}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {25, 160}, NULL },
  { RESOURCE_ID_VERT_BUSH_35, {0, 160}, NULL },
  { RESOURCE_ID_VERT_BUSH_35, {15, 185}, NULL },
  { RESOURCE_ID_VERT_BUSH_35, {240, 285}, NULL },
  { RESOURCE_ID_SHRUB_8, {54, 204}, NULL },
  { RESOURCE_ID_SHRUB_8, {62, 204}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_24, {70, 196}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_24, {46, 196}, NULL },
  { RESOURCE_ID_SHRUB_8, {54, 188}, NULL },
  { RESOURCE_ID_SHRUB_8, {62, 188}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_24, {160, 106}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_24, {136, 106}, NULL },
  { RESOURCE_ID_SHRUB_8, {112, 106}, NULL },
  { RESOURCE_ID_SHRUB_8, {128, 106}, NULL },
  { RESOURCE_ID_SHRUB_8, {120, 106}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {112, 114}, NULL },
  { RESOURCE_ID_SHRUB_8, {120, 130}, NULL },
  { RESOURCE_ID_SHRUB_8, {128, 130}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {136, 130}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {144, 146}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {152, 146}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {160, 154}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {160, 178}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {160, 202}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_24, {160, 234}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {184, 202}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {184, 178}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {184, 154}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {184, 130}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {184, 106}, NULL },
  { RESOURCE_ID_SHRUB_8, {112, 242}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_24, {120, 242}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_24, {144, 242}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_24, {168, 242}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {192, 226}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {184, 226}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {160, 218}, NULL },
};

static LevelSpriteLocation level0_scenery[] = {
  { RESOURCE_ID_VERT_WATER_24, {176, 210}, NULL },
  { RESOURCE_ID_VERT_WATER_24, {168, 210}, NULL },
  { RESOURCE_ID_VERT_WATER_24, {176, 186}, NULL },
  { RESOURCE_ID_VERT_WATER_24, {168, 186}, NULL },
  { RESOURCE_ID_VERT_WATER_24, {176, 162}, NULL },
  { RESOURCE_ID_VERT_WATER_24, {168, 162}, NULL },
  { RESOURCE_ID_VERT_WATER_24, {176, 138}, NULL },
  { RESOURCE_ID_VERT_WATER_24, {168, 138}, NULL },
  { RESOURCE_ID_VERT_WATER_24, {176, 114}, NULL },
  { RESOURCE_ID_VERT_WATER_24, {168, 114}, NULL },
  { RESOURCE_ID_HORIZ_WATER_24, {144, 154}, NULL },
  { RESOURCE_ID_HORIZ_WATER_24, {144, 146}, NULL },
  { RESOURCE_ID_HORIZ_WATER_24, {144, 138}, NULL },
  { RESOURCE_ID_HORIZ_WATER_24, {144, 130}, NULL },
  { RESOURCE_ID_HORIZ_WATER_24, {144, 122}, NULL },
  { RESOURCE_ID_HORIZ_WATER_24, {144, 114}, NULL },
  { RESOURCE_ID_HORIZ_WATER_24, {120, 122}, NULL },
  { RESOURCE_ID_HORIZ_WATER_24, {120, 114}, NULL },
};

static LevelSpriteLocation level0_walls[] = {
  { RESOURCE_ID_HORIZ_WALL_144, {0, 0}, NULL },
  { RESOURCE_ID_HORIZ_WALL_144, {144, 0}, NULL },
  { RESOURCE_ID_HORIZ_WALL_144, {0, 328}, NULL },
  { RESOURCE_ID_HORIZ_WALL_144, {144, 328}, NULL },
  { RESOURCE_ID_VERT_WALL_168, {0, 0}, NULL },
  { RESOURCE_ID_VERT_WALL_168, {0, 168}, NULL },
  { RESOURCE_ID_VERT_WALL_168, {280, 0}, NULL },
  { RESOURCE_ID_VERT_WALL_168, {280, 168}, NULL },
};

// Track layout - drawn in order of index in array
static LevelSpriteLocation level0_tracks[] = {
  { RESOURCE_ID_CURVE_ROAD_NW, {50, 50}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NW, {50, 200}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NE, {200, 50}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NE, {100, 150}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SE, {200, 250}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SE, {100, 200}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SW, {50, 150}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SW, {50, 250}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_50, {100, 50}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_50, {150, 50}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_50, {100, 250}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_50, {150, 250}, NULL },
  { RESOURCE_ID_VERT_ROAD_50, {50, 100}, NULL },
  { RESOURCE_ID_VERT_ROAD_50, {200, 100}, NULL },
  { RESOURCE_ID_VERT_ROAD_50, {200, 150}, NULL },
  { RESOURCE_ID_VERT_ROAD_50, {200, 200}, NULL },
};

static LevelSpriteLocation level0_checkpoints[] = {
  { RESOURCE_ID_CHECKPOINT_VERT_70, {165, 255}, NULL },
  { RESOURCE_ID_CHECKPOINT_HORIZ_70, {200, 115}, NULL },
};

static LevelFinishLine level0_finish_group = {
  { RESOURCE_ID_FINISH_LINE_VERT, {100, 55}, NULL },
  { RESOURCE_ID_FINISH_BOX_1, {115, 30}, NULL },
  { RESOURCE_ID_FINISH_BOX_2, {135, 30}, NULL },
  { RESOURCE_ID_FINISH_BOX_3, {155, 30}, NULL },
  { RESOURCE_ID_FINISH_BOX_4, {175, 30}, NULL },
  { RESOURCE_ID_LIGHT_SIGNAL_ALL, {105, 30}, NULL },
};

static GPoint level0_track_points0[] = {
  {90, 70},
  {75, 75},
  {70, 90},
  {70, 105},
  {70, 140},
  {70, 155},
  {75, 170},
  {90, 175},
  {105, 170},
  {120, 175},
  {125, 190},
  {125, 205},
  {120, 220},
  {105, 225},
  {90, 220},
  {75, 225},
  {70, 240},
  {70, 255},
  {75, 270},
  {90, 275},
  {105, 275},
  {140, 275},
  {155, 275},
  {190, 275},
  {205, 275},
  {220, 270},
  {225, 255},
  {225, 240},
  {225, 205},
  {225, 190},
  {225, 155},
  {225, 140},
  {225, 105},
  {225, 90},
  {220, 75},
  {205, 70},
  {190, 70},
  {155, 70},
  {140, 70},
  {105, 70},
};

static GPoint level0_car_locations[] = {
  {145, 55},
  {145, 75},
  {120, 55},
  {120, 75},
};

static LevelSpriteDetails level0_details = {
  .level = LEVEL0_ID,
  .sprites = level0_sprites,
  .scenery = level0_scenery,
  .walls = level0_walls,
  .tracks = level0_tracks,
  .checkpoints = level0_checkpoints,
  .finish_group = &level0_finish_group,
  .track_points0 = level0_track_points0,
  .car_locations = level0_car_locations,
  .car_starting_angle = LEVEL0_CAR_STARTING_ANGLE,
};
